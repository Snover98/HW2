#ifndef __HW2__
#define __HW2__

#include "tokens.h"
#include "grammar.h"

//gets lhs and rhs of a rule, return true if the given lhs is nullable
//go over all the rules begin with the given lhs and checks if one of them is nullable
bool is_nullable (nonterminal curr_lhs){
    if (has_nullable_rule(curr_lhs)){
        return true;
    }
    //go over all the rules in the grammer
    for (std::vector<grammar_rule>::iterator it_g = grammar.begin() ; it_g != grammar.end(); ++it_g) {
        //check if the current rule begin with the given lhs
        if (curr_lhs!=(*it_g).lhs){
            continue;
        }
        for (std::vector<int>::iterator it = (*it_g).rhs.begin(); it != (*it_g).rhs.end(); ++it) {
            // if the rhs of a rule contains a token or it isn't nullable then the rule isn't nullable
            if ((*it) > NONTERMINAL_ENUM_SIZE || !is_nullable(*it)) {
                break;
            }
            //if we went over all the rhs and it all contains nullable variables then the lhs is nullable
            if ((*it) ==(*it_g).rhs.end()){
                return true;
            }
        }
    }
    return false;
}

//helper function for is_nullable. gets a nonterminal N and return true if it has a rule: N->epsilon
bool has_nullable_rule(nonterminal N){
    //go over all the rules in the grammer
    for (std::vector<grammar_rule>::iterator it_g = grammar.begin() ; it_g != grammar.end(); ++it){
        //if the rule lhs is as the given nonterminal and the rhl is empty (hence epsilon)
        if ((N==(*it_g).lhs) && (((*it_g).rhs)).empty()){
            return true;
        }
    }
    return false;
}


/**
 * determines which variables are nullable, i.e. can produce an empty word
 * calls print_nullable when finished
 */
void compute_nullable(){
    std::vector<bool> vec;
    for (std::vector<grammar_rule>::iterator it = grammar.begin() ; it != grammar.end(); ++it){
        vec.push_back(is_nullable((*it).lhs));
    }
    print_nullable(&vec);
}

/**
 * computes first for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_first when finished
 */
void compute_first();

/**
 * computes follow for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_follow when finished
 */
void compute_follow();

/**
 * computes select for all grammar rules (see grammar global variable in grammar.h)
 * calls print_select when finished
 */
void compute_select();

/**
 * implements an LL(1) parser for the grammar using yylex()
 */
void parser();
/*
 * implemented in lex.yy.c (generated from lexer.lex)
 */
int yylex();

#endif //__HW2__
