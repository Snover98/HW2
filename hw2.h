#ifndef __HW2__
#define __HW2__

#include "tokens.h"
#include "grammar.h"

//gets lhs and rhs of a rule, return true if the given lhs is nullable
//go over all the rules begin with the given lhs and checks if one of them is nullable
bool is_nullable (nonterminal curr_lhs){
    //go over all the rules in the grammer
    for (std::vector<grammar_rule>::iterator it_g = grammar.begin() ; it_g != grammar.end(); ++it_g) {
        //check if the current rule begin with the given lhs
        if (curr_lhs!=(*it_g).lhs){
            continue;
        }

        bool bad = false;
        for(std::vector<int>::iterator it = (*it_g).rhs.begin(); it != (*it_g).rhs.end(); ++it){
            if ((*it) > NONTERMINAL_ENUM_SIZE || !is_nullable(*it)) {
                bad = true;
                break;
            }
        }

        if(!bad){
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
    std::vector<bool> vec(NONTERMINAL_ENUM_SIZE, false);
    for (std::vector<grammar_rule>::iterator it = grammar.begin() ; it != grammar.end(); ++it){
        if (is_nullable((*it).lhs)){
            vec[(*it).lhs] = true;
        }
    }
    print_nullable(vec);
}

void updateSet(std::set<tokens>& dest, token item, bool* b){
    if(dest.find(item)==dest.end()) {
        *b = false;
        dest.insert(*it);
    }
}

//copy all the elements in the set source to the set dest
void insert_set (std::set<tokens>& dest, std::set<tokens> & source, bool* b){
    for (std::set<tokens>::iterator it = source.begin() ; it != source.end(); ++it){
        updateSet(dest, *it, b);
    }
}

void compute_vector_first(std::vector< std::set<tokens> >& first){
    //initiate first to contain NONTERMINAL_ENUM_SIZE empty sets
    for (int i=0;i<NONTERMINAL_ENUM_SIZE;i++){
        first.push_back({});
    }

    bool finished=false;//false when any more changes accures

    while(!finished){
        finished=true;
        //going over all the grammar
        for (std::vector<grammar_rule>::iterator it_g = grammar.begin() ; it_g != grammar.end(); ++it_g) {
            for (std::vector<int>::iterator it = (*it_g).rhs.begin(); it != (*it_g).rhs.end(); ++it){
                if ((*it)>NONTERMINAL_ENUM_SIZE){//checks it the curr location in the rhs is a terminal
                    updateSet(first[(*it_g).lhs], *it, &finished);//for X->aY : first(X)<-first(X) U a
                    // first[(*it_g).lhs].insert(*it);
                    break;
                }
                insert_set(first[(*it_g).lhs], first[*it], &finished); //for X->AB : first(X)<-first(A) U first(B)
                if (!is_nullable(*it)){//checks if the current nonterminal of the rhs is nullable
                    break;
                }
            }
        }
    }
    return first;
}

/**
 * computes first for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_first when finished
 */
void compute_first(){
    std::vector< std::set<tokens> > first;
    compute_vector_first(first);
    print_first(first);
}

void compute_vector_follow(std::vector< std::set<tokens> >& follow){
    std::vector< std::set<tokens> > first;
    compute_vector_first(first);
    //initiate follow to contain NONTERMINAL_ENUM_SIZE empty sets
    for (int i = 0; i < NONTERMINAL_ENUM_SIZE; i++) {
        follow.push_back({});
    }
    (follow[0]).insert(EF); // TODO check if right

    bool finished = false;//false when any more changes accures

    while (!finished) {
        finished = true;
        //going over all the nonterminal
        for (int i = 0; i < NONTERMINAL_ENUM_SIZE; i++) {
            //going over all the grammar
            for (std::vector<grammar_rule>::iterator it_g = grammar.begin(); it_g != grammar.end(); ++it_g) {
                for (std::vector<int>::iterator it = (*it_g).rhs.begin(); it != (*it_g).rhs.end(); ++it) {
                    if ((*it) == i && (it + 1) != (*it_g).rhs.end()) {
                        insert_set(follow[i], first[*(it + 1)], &finished);
                    }
                    if ((it + 1) != (*it_g).rhs.end() && is_nullable(*(it + 1))) {
                        insert_set(follow[i], follow[(*it_g).lhs], &finished);
                    }
                }
            }
        }
    }
}


/**
 * computes follow for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_follow when finished
 */
void compute_follow() {
    std::vector<std::set<tokens> > follow;
    compute_vector_follow(follow);
    print_follow(follow);
}

/**
 * computes select for all grammar rules (see grammar global variable in grammar.h)
 * calls print_select when finished
 */
void compute_select(){
//    std::vector<std::set<tokens> > follow;
//    compute_vector_follow(follow);
//    std::vector< std::set<tokens> > first;
//    compute_vector_first(first);
//    std::vector< std::set<tokens> > select;
//    int i=0;
//    bool finish=false;
//    for (std::vector<grammar_rule>::iterator it_g = grammar.begin(); it_g != grammar.end(); ++it_g){
//        insert_set(select[i],first[*((*it_g).rhs.begin())],&finish);
//        if (is_nullable((*it_g).rhs.begin())){
//            insert_set(select[i],follow[(*it_g).lhs],&finish);
//        }
//    }
}

/**
 * implements an LL(1) parser for the grammar using yylex()
 */
void parser();
/*
 * implemented in lex.yy.c (generated from lexer.lex)
 */
int yylex();

#endif //__HW2__
