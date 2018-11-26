%{
/*C lang stuff*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "tokens.h"

#define NON_ESCAPE 'E'
#define HEXADECIMAL 'X'

tokens showToken(char* token, tokens token_type);
tokens showInteger(int base);
tokens showString();
char hexToNum(char hex);
char convertEscape(char e);
void badInput(char* input);
void printTokenString(char* token, char* value);
void printTokenInt(char* token, long value);
bool isPrintable(char c);
%}

%option noyywrap
%option yylineno
%option caseless

%s KEYCOND
%s ASSORIND
%x UNBLSTR

digit ([0-9])
hex ([0-9a-f])
letter ([a-zA-Z])
key_legal [~\$\-_?'\.]
file ([/]({letter}([\-\+\._~]|{letter}|{digit})*)?)
key {letter}(({letter}|{digit}|{key_legal}|[ ])*({letter}|{digit}|{key_legal}))*
hexadecimal 0x({hex}*)
escape (\\([\\\"abnrt0;:=#]|x{hex}{2}))
unblockedString (({letter})(([ -\"$-+\--:<-~\t]*)([!-\"$-+\--:<-~]))?)

%%

^{key}																				{ /*KEY*/
																						showToken("KEY");
																						BEGIN(KEYCOND);
																					}
																					
\[{key}\]																			return showToken("SECTION", SECTION); /*SECTION*/

^([\t ]+)/([^#;\t ])																{ /*INDENT*/
																						showToken("INDENT");
																						BEGIN(ASSORIND);
																					}


<*>[\t ]+																			; /*WHITESPACE*/

<*>[\n\r]+																			BEGIN(0); /*NEWLINE*/

<KEYCOND>([:=])/([ \t]*[:=])														return showToken("ASSIGN", ASSIGN);

<KEYCOND>([:=])																		{ /*ASSIGN*/
																						BEGIN(ASSORIND);
																						return showToken("ASSIGN", ASSIGN);
                                                                                    }

(([#;])((.)*[^ \t\r\n])?)/(([\t ]*))												showToken("COMMENT", EF); /*COMMENT*/

<<EOF>>																				{
																						return showToken("EOF", EF); /*EOF*/
																						exit(0);
																					}

<ASSORIND>((true)|(yes))															return showToken("TRUE", TRUE); /*THESE ONES MAY ONLY APPEAR AFTER ASSIGN OR INDENT*/ /*TRUE*/

<ASSORIND>((false)|(no))															return showToken("FALSE", FALSE); /*FALSE*/

<ASSORIND>([\+\-]?([0-9]*(\.)[0-9]+)(e([\+-])[0-9]+)?)								return showToken("REAL", REAL); /*REAL*/
<ASSORIND>([\+\-]?([0-9]+(\.)[0-9]*)(e([\+-])[0-9]+)?)								return showToken("REAL", REAL); /*REAL*/

<ASSORIND>({hexadecimal})															return showInteger(16); /*INTEGER - hexadecimal number*/

<ASSORIND>("0b"[01]*)																return showInteger(2); /*INTEGER - binary number*/

<ASSORIND>(0[0-7]*)																	return showInteger(8); /*INTEGER - octal number*/

<ASSORIND>([\+\-]?{digit}+)															return showInteger(10); /*INTEGER - decimal number*/

<ASSORIND>({file}+)																	return showToken("PATH", PATH); /*PATH*/

<ASSORIND>(\$\{({key}#)?{key}\})													return showToken("LINK", LINK); /*LINK*/

<ASSORIND>((\")([^\"\\]|{escape})*(\"))												return showString();

<ASSORIND>((\")(([^\"]))*[\"]?)														return showString();

<ASSORIND>{letter}																	{
																						BEGIN(UNBLSTR);
																						yyless(0);
																					}

<UNBLSTR>{unblockedString}															{/*unblocked string*/
																						BEGIN(ASSORIND);
																						return showToken("STRING");
																					}

<ASSORIND>[,]																		return showToken("SEP", SEP); /*SEP*/

<*>.																				badInput(yytext); /*DEFAULT*/
%%

/*handle integer token*/
tokens showInteger(int base){
	/*the actual number*/
	long integer = 0;

	/*if it's binary, we should handle it with a special way, as strtol doesn't do binary well*/
	if(base == 2){
		/*convert the lexame after the 0b into a long int*/
		integer = strtol(yytext+2, NULL, 2);
	}
	else{ /*if it isn't a binary number*/
		integer = strtol(yytext, NULL, base);
	}


	/*print the integer in decimal*/
	printTokenInt("INTEGER",integer);

//    yylval.ival = integer;
	return INTEGER;
}

/*boolean function that tells you if a char is printable*/
bool isPrintable(char c){
	return ( '\x20' <= c && c <= '\x7e' || c == '\t' || c == '\r' || c == '\n');
}

/*handle string token*/
tokens showString(){
	char* copy_lexame;
	char* cur_ptr;
	char cur_val = 0;
	int i;
	bool is_finished = false;

	/*allocate memory for copying the lexame*/
	copy_lexame = (char*)malloc(yyleng*sizeof(char));
	cur_ptr = copy_lexame;


	for (i = 1; i < yyleng-1; i++){
		/*save current char*/
		cur_val = yytext[i];

		/*if the current character isn't printable*/
		if(!isPrintable(cur_val)){
			free(copy_lexame);
			printf("Error %c\n",cur_val);
			exit(0);
		}

		/*if we have a case of CRLF, skip to the next char*/
		if(cur_val == '\r' && yytext[i+1] == '\n'){
			continue;
		}

		/*check for escape sequences*/
		if(cur_val == '\\'){
			/*convert the current val to the correct thing, case-insensitive*/
			cur_val = convertEscape(tolower(yytext[i+1]));

			/*in case of a non valid escape sequence*/
			if(cur_val == NON_ESCAPE){
				printf("Error undefined escape sequence %c\n", yytext[i+1]);
				free(copy_lexame);
				exit(0);
			}

			/*in case of a hexadecimal value*/
			if(cur_val == HEXADECIMAL){
				/*check for invalid values and incomplete hexes*/
				if(i+3 >= yyleng || !isxdigit(yytext[i+2]) || !isxdigit(yytext[i+3])){
					printf("Error undefined escape sequence x\n");
					free(copy_lexame);
					exit(0);
				}

				/*make the escape val correct*/
				cur_val = 16*hexToNum(tolower(yytext[i+2])) + hexToNum(tolower(yytext[i+3]));

				/*advance i to the correct place (minus the part always done later)*/
				i += 2;

			}

			/*special case of when the string ends with \"*/
			if(i == yyleng-2 && cur_val == '\"'){
				printf("Error unclosed string\n");
				free(copy_lexame);
				exit(0);
			}

			/*advance i to the correct place*/
			++i;

			/*if the current char is '\0', i.e the string should end with it*/
			if(cur_val == '\0'){
				/*change is_finished so that we won't add any more chars to the string*/
				is_finished = true;

			}

		} else if(cur_val == '\n' || cur_val == '\r'){	/*replace new line with space*/
			cur_val=' ';
		}

		/*if we aren't finished copy the correct value to the copy and advance the pointer*/
		if(!is_finished){
			*cur_ptr = cur_val;
			++cur_ptr;
		}
	}

	if(yytext[yyleng-1] == '\\'){
		free(copy_lexame);
		badInput("\\");
	}

	/*check if the string is blocked*/
	if(yytext[yyleng-1] != '\"'){
		printf("Error unclosed string\n");
		free(copy_lexame);
		exit(0);
	}

	/*put an \0 at the end of the string*/
	*cur_ptr = '\0';

	printTokenString("STRING", copy_lexame);

	free(copy_lexame);

//    yylval.text = copy_lexame;

    return STRING;
}

/*converts hexadecimal digit to the actual num*/
char hexToNum(char hex){
	if('0' <= hex && hex <= '9'){
		return hex - '0';
	} else{
		return hex - 'a' + 10;
	}
}

/*escape chararcter recognition*/
char convertEscape(char e){
	switch(e){
		case '\\':
			return '\\';

		case '\"':
			return '\"';

		case 'a':
			return '\a';

		case 'b':
			return '\b';

		case 'n':
			return '\n';

		case 'r':
			return '\r';

		case 't':
			return '\t';

		case '0':
			return '\0';

		case ';':
		case ':':
		case '=':
		case '#':
			return e;

		case 'x':
			return HEXADECIMAL;

		default:
			return NON_ESCAPE;
	}
}


tokens showToken(char* token, tokens token_type){
	/*print the token*/
	printTokenString(token,yytext);

//	char* copy_lexame = (char*)malloc(yyleng*sizeof(char));
//
//    yylval.text = copy_lexame;

    return token_type;
}

/*print error and exit when get illegal input*/
void badInput(char* input){
	printf("Error %s\n",input);
	exit(0);
}

/*print the output*/
void printTokenString(char* token, char* value){
	/*If there was no error, print the row number, the token and then the lexame*/
	printf("%d %s %s\n", yylineno, token, value);
}

/*print the output for integers*/
void printTokenInt(char* token, long value){
	/*If there was no error, print the row number, the token and then the lexame*/
	printf("%d %s %ld\n", yylineno, token, value);
}

