//
// Created by user on 22/11/2018.
//

#include <map>
#include "hw2.h"


//gets lhs and rhs of a rule, return true if the given lhs is nullable
//go over all the rules begin with the given lhs and checks if one of them is nullable
bool is_nullable (nonterminal curr_lhs){
    //go over all the rules in the grammer
    for (std::vector<grammar_rule>::iterator it_g = grammar.begin() ; it_g != grammar.end(); ++it_g) {
        //check if the current rule begin with the given lhs
        if (curr_lhs!=(*it_g).lhs){
            continue;
        }

        bool bad = false;// true if we get a token or a not nullable nonterminal
        for(std::vector<int>::iterator it = (*it_g).rhs.begin(); it != (*it_g).rhs.end(); ++it){
            if ((*it) >= NONTERMINAL_ENUM_SIZE || !is_nullable((nonterminal )*it)) {
                bad = true;
                break;
            }
        }

        //if we got to a varible is nut nullable
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
    // go over all the grammar
    for (std::vector<grammar_rule>::iterator it = grammar.begin() ; it != grammar.end(); ++it){
        if (is_nullable((*it).lhs)){
            vec[(*it).lhs] = true;
        }
    }
    print_nullable(vec);
}

//gets a set and an item to insert the set. if the item was already in the set b=true, otherwise b=false
void updateSet(std::set<tokens>& dest, tokens item, bool* b){
    if(dest.find(item) == dest.end()) {
        *b = false;
        dest.insert(item);
    }
}

//copy all the elements in the set source to the set dest . if there was an item in source that wasn't already in dest b=false, otherwise b=true
void insert_set (std::set<tokens>& dest, std::set<tokens> & source, bool* b){
    for (std::set<tokens>::iterator it = source.begin() ; it != source.end(); ++it){
        updateSet(dest, *it, b);
    }
}

//computes vector of all first sets
void compute_vector_first(std::vector< std::set<tokens> >& first){
    //initiate first to contain NONTERMINAL_ENUM_SIZE empty sets
    for (int i=0;i<NONTERMINAL_ENUM_SIZE;i++){
        first.push_back({});
    }

    bool finished=false;//false when any more changes accure

    while(!finished){
        finished=true;
        //going over all the grammar
        for (std::vector<grammar_rule>::iterator it_g = grammar.begin() ; it_g != grammar.end(); ++it_g) {
            for (std::vector<int>::iterator it = (*it_g).rhs.begin(); it != (*it_g).rhs.end(); ++it){
                if ((*it)>NONTERMINAL_ENUM_SIZE){//checks it the curr location in the rhs is a terminal
                    updateSet(first[(*it_g).lhs], (tokens )(*it), &finished);//for X->aY : first(X)<-first(X) U a
                    break;
                }
                insert_set(first[(*it_g).lhs], first[*it], &finished); //for X->AB : first(X)<-first(X) U first(A)
                if (!is_nullable((nonterminal (*it)))){//checks if the current nonterminal of the rhs is nullable
                    break;
                }
            }
        }
    }
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

//computes vector of all follow sets
void compute_vector_follow(std::vector< std::set<tokens> >& follow){
    std::vector< std::set<tokens> > first;
    compute_vector_first(first);
    //initiate follow to contain NONTERMINAL_ENUM_SIZE empty sets
    for (int i = 0; i < NONTERMINAL_ENUM_SIZE; i++) {
        follow.push_back({});
    }
    //first varible must contain EOF (=$)
    (follow[S]).insert(EF);

    bool finished = false;//false when any more changes accures

    while (!finished) {
        finished = true;
        //going over all the nonterminal
        for (int i = 0; i < NONTERMINAL_ENUM_SIZE; i++) {
            //going over all the grammar
            for (std::vector<grammar_rule>::iterator it_g = grammar.begin(); it_g != grammar.end(); ++it_g) {
                //go over the rhs of a rule
                for (std::vector<int>::iterator it = (*it_g).rhs.begin(); it != (*it_g).rhs.end(); ++it) {
                    if ((*it) == i) { // if we got to a place in a certain rule in which the currn't nonterminal exists
                        std::vector<int>::iterator it_copy = it;
                        ++it_copy;
                        while (it_copy != (*it_g).rhs.end()) { // going over the rest of the rhs
                            if (*(it_copy) >  NONTERMINAL_ENUM_SIZE){ // if we got to a terminal
                                updateSet(follow[i],(tokens)(*(it_copy)),&finished); // insert the terminal to the follow
                                break;
                            }
                            else{
                                insert_set(follow[i], first[*(it_copy)], &finished);
                                if (!is_nullable((nonterminal)(*(it_copy)))){ // if the current nonterminal is not nullable
                                    break;
                                }
                            }
                            ++it_copy;
                        }
                        if (it_copy==(*it_g).rhs.end()){ // if all the rest of the rhs is nullable
                            insert_set(follow[i], follow[(*it_g).lhs], &finished); // add the follow of the current rule lhs
                        }
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
    std::vector<std::set<tokens> > follow;
    compute_vector_follow(follow);
    std::vector< std::set<tokens> > first;
    compute_vector_first(first);
    std::vector< std::set<tokens> > select;
    std::set<tokens> curr_rule_sel={};

    bool finish=false; //not really necessary
    bool rhs_nullable=true;
    //going over all the rules
    for (std::vector<grammar_rule>::iterator it_g = grammar.begin(); it_g != grammar.end(); ++it_g){
        curr_rule_sel={};
        rhs_nullable=true;
        for (std::vector<int>::iterator it = (*it_g).rhs.begin(); it != (*it_g).rhs.end(); ++it){ //going over the rhs of a rule
            if (*it > NONTERMINAL_ENUM_SIZE){ // if the current varible is token
                updateSet(curr_rule_sel,(tokens) (*(it)),&finish); // insert the token to the select of the current rule
                rhs_nullable=false;
                break;
            }
            else{
                insert_set(curr_rule_sel,first[*it],&finish); //  insert the nonterminal to the select of the current rule
                if (!is_nullable((nonterminal)*it)){ // if the nonterminal is not nullable 
                    rhs_nullable=false;
                    break;
                }
            }
        }
        if (rhs_nullable){
            insert_set(curr_rule_sel,follow[(*it_g).lhs],&finish);
        }
        select.push_back(curr_rule_sel);
    }
    print_select(select);
}


/**
 * implements an LL(1) parser for the grammar using yylex()
 */
void parser(){
    //the stack Q and the table M
    std::vector<int> Q = std::vector<int>();
    std::map<nonterminal, std::map<tokens, int>> M = std::map<nonterminal, std::map<tokens, int>>();

    //vectors of terminals and nonterminals for convinience
    std::vector<tokens> T {KEY, SECTION, INDENT, ASSIGN, TRUE, FALSE, INTEGER, REAL, STRING, PATH, LINK, SEP, EF};
    std::vector<nonterminal> V {S, Sections, Section, Keys, Key, Assign, Values, MoreValues, IndentedValues, Value};

    //initialize the map M
    //for each nonterminal
    for(auto const& X: V){
        //for each terminal
        for(auto const& t: T){
            //for each rule in the grammar that starts with X
            for(int rule_num = 1; rule_num <= grammar.size(); rule_num++){
                //if is in select of the rule
                //@TODO make a function that checks if t is in r's select
                if(grammar[rule_num-1].lhs == X && t in select(grammar[rule_num-1])){
                    M[X][t] = rule_num;
                }
            }
        }
    }








}


