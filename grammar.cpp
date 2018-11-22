#include "tokens.h"
#include "grammar.h"
#include "hw2.h"
#include <iostream>

void print_token(tokens tok) {
    switch (tok) {
        case KEY: std::cout << "KEY"; break;
        case SECTION: std::cout << "SECTION"; break;
        case INDENT: std::cout << "INDENT"; break;
        case ASSIGN: std::cout << "ASSIGN"; break;
        case TRUE: std::cout << "TRUE"; break;
        case FALSE: std::cout << "FALSE"; break;
        case INTEGER: std::cout << "INTEGER"; break;
        case REAL: std::cout << "REAL"; break;
        case STRING: std::cout << "STRING"; break;
        case PATH: std::cout << "PATH"; break;
        case LINK: std::cout << "LINK"; break;
        case SEP: std::cout << "SEP"; break;
        case EF: std::cout << "EOF"; break;
    };
}

void print_nonterminal(nonterminal var) {
    switch (var) {
        case S: std::cout << "S"; break;
        case Sections: std::cout << "Sections"; break;
        case Section: std::cout << "Section"; break;
        case Keys: std::cout << "Keys"; break;
        case Key: std::cout << "Key"; break;
        case Assign: std::cout << "Assign"; break;
        case Values: std::cout << "Values"; break;
        case MoreValues: std::cout << "MoreValues"; break;
        case IndentedValues: std::cout << "IndentedValues"; break;
        case Value: std::cout << "Value"; break;
    };
}

void print_nullable(const std::vector<bool>& vec) {
    std::cout << "Nullable:" << std::endl;
    for (int i = 0; i < NONTERMINAL_ENUM_SIZE; ++i) {
        print_nonterminal((nonterminal) (i));
        std::cout << " = ";
        if (vec[i]) std::cout << "true"; else std::cout << "false";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void print_first(const std::vector< std::set<tokens> >& vec) {
    std::cout << "First:" << std::endl;
    for (int i = 0; i < NONTERMINAL_ENUM_SIZE; ++i) {
        print_nonterminal((nonterminal) (i));
        std::cout << " = { ";
        for (std::set<tokens>::const_iterator it = vec[i].begin(); it != vec[i].end(); ++it) {
            if (it != vec[i].begin()) std::cout << " , ";
            print_token(*it);
        }
        std::cout << " }";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void print_follow(const std::vector< std::set<tokens> >& vec) {
    std::cout << "Follow:" << std::endl;
    for (int i = 0; i < NONTERMINAL_ENUM_SIZE; ++i) {
        print_nonterminal((nonterminal) (i));
        std::cout << " = { ";
        for (std::set<tokens>::const_iterator it = vec[i].begin(); it != vec[i].end(); ++it) {
            if (it != vec[i].begin()) std::cout << " , ";
            print_token(*it);
        }
        std::cout << " }";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void print_select(const std::vector< std::set<tokens> >& vec) {
    std::cout << "Select:" << std::endl;
    for (int i = 0; i < grammar.size(); ++i) {
        grammar_rule gr = grammar[i];
        std::cout << "( ";
        print_nonterminal((nonterminal) gr.lhs);
        std::cout << " -> ";
        for (int j = 0; j < gr.rhs.size(); ++j) {
            if (j > 0) std::cout << " ";
            if (gr.rhs[j] >= NONTERMINAL_ENUM_SIZE)
                print_token((tokens) gr.rhs[j]);
            else
                print_nonterminal((nonterminal) gr.rhs[j]);
        }
        std::cout << " ) = { ";
        for (std::set<tokens>::const_iterator it = vec[i].begin(); it != vec[i].end(); ++it) {
            if (it != vec[i].begin()) std::cout << " , ";
            print_token(*it);
        }
        std::cout << " }";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


std::vector <grammar_rule> make_grammar() {
    std::vector <grammar_rule> res;

    res.push_back(grammar_rule(S, std::vector<int>(1, Sections)));

    int rule2[] = {Section, Sections};
    res.push_back(grammar_rule(Sections, std::vector<int>(rule2, rule2 + 2)));
    res.push_back(grammar_rule(Sections, std::vector<int>()));

    int rule4[] = {SECTION, Keys};
    res.push_back(grammar_rule(Section, std::vector<int>(rule4, rule4 + 2)));

    int rule5[] = {Key, Keys};
    res.push_back(grammar_rule(Keys, std::vector<int>(rule5, rule5 + 2)));
    res.push_back(grammar_rule(Keys, std::vector<int>()));

    int rule7[] = {KEY, Assign};
    res.push_back(grammar_rule(Key, std::vector<int>(rule7, rule7 + 2)));

    int rule8[] = {ASSIGN, Values};
    res.push_back(grammar_rule(Assign, std::vector<int>(rule8, rule8 + 2)));
    res.push_back(grammar_rule(Assign, std::vector<int>()));

    int rule10[] = {IndentedValues, MoreValues};
    res.push_back(grammar_rule(Values, std::vector<int>(rule10, rule10 + 2)));

    int rule11[] = {SEP, IndentedValues, MoreValues};
    res.push_back(grammar_rule(MoreValues, std::vector<int>(rule11, rule11 + 3)));
    int rule12[] = {INDENT, SEP, IndentedValues, MoreValues};
    res.push_back(grammar_rule(MoreValues, std::vector<int>(rule12, rule12 + 4)));
    res.push_back(grammar_rule(MoreValues, std::vector<int>()));

    res.push_back(grammar_rule(IndentedValues, std::vector<int>(1, Value)));
    int rule15[] = {INDENT, Value};
    res.push_back(grammar_rule(IndentedValues, std::vector<int>(rule15, rule15 + 2)));

    res.push_back(grammar_rule(Value, std::vector<int>(1, STRING)));
    res.push_back(grammar_rule(Value, std::vector<int>(1, INTEGER)));
    res.push_back(grammar_rule(Value, std::vector<int>(1, REAL)));
    res.push_back(grammar_rule(Value, std::vector<int>(1, PATH)));
    res.push_back(grammar_rule(Value, std::vector<int>(1, TRUE)));
    res.push_back(grammar_rule(Value, std::vector<int>(1, FALSE)));
    res.push_back(grammar_rule(Value, std::vector<int>(1, LINK)));

    return res;
}

std::vector <grammar_rule> grammar = make_grammar();
