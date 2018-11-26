//
// Created by user on 22/11/2018.
//

#include <map>
#include <iostream>
#include "hw2.h"


//gets lhs and rhs of a rule, return true if the given lhs is nullable
//go over all the rules begin with the given lhs and checks if one of them is nullable
bool is_nullable(nonterminal curr_lhs) {
    //go over all the rules in the grammer
    for (std::vector<grammar_rule>::iterator it_g = grammar.begin(); it_g != grammar.end(); ++it_g) {
        //check if the current rule begin with the given lhs
        if (curr_lhs != (*it_g).lhs) {
            continue;
        }

        bool bad = false;// true if we get a token or a not nullable nonterminal
        for (std::vector<int>::iterator it = (*it_g).rhs.begin(); it != (*it_g).rhs.end(); ++it) {
            if ((*it) >= NONTERMINAL_ENUM_SIZE || !is_nullable((nonterminal) *it)) {
                bad = true;
                break;
            }
        }

        //if we got to a varible is nut nullable
        if (!bad) {
            return true;
        }

    }
    return false;
}


/**
 * determines which variables are nullable, i.e. can produce an empty word
 * calls print_nullable when finished
 */
void compute_nullable() {
    std::vector<bool> vec(NONTERMINAL_ENUM_SIZE, false);
    // go over all the grammar
    for (std::vector<grammar_rule>::iterator it = grammar.begin(); it != grammar.end(); ++it) {
        if (is_nullable((*it).lhs)) {
            vec[(*it).lhs] = true;
        }
    }
    print_nullable(vec);
}

//gets a set and an item to insert the set. if the item was already in the set b=true, otherwise b=false
void updateSet(std::set<tokens> &dest, tokens item, bool *b) {
    if (dest.find(item) == dest.end()) {
        *b = false;
        dest.insert(item);
    }
}

//copy all the elements in the set source to the set dest . if there was an item in source that wasn't already in dest b=false, otherwise b=true
void insert_set(std::set<tokens> &dest, std::set<tokens> &source, bool *b) {
    for (std::set<tokens>::iterator it = source.begin(); it != source.end(); ++it) {
        updateSet(dest, *it, b);
    }
}

//computes vector of all first sets
void compute_vector_first(std::vector<std::set<tokens> > &first) {
    //initiate first to contain NONTERMINAL_ENUM_SIZE empty sets
    for (int i = 0; i < NONTERMINAL_ENUM_SIZE; i++) {
        first.push_back(std::set<tokens>());
    }

    bool finished = false;//false when any more changes accure

    while (!finished) {
        finished = true;
        //going over all the grammar
        for (std::vector<grammar_rule>::iterator it_g = grammar.begin(); it_g != grammar.end(); ++it_g) {
            for (std::vector<int>::iterator it = (*it_g).rhs.begin(); it != (*it_g).rhs.end(); ++it) {
                if ((*it) > NONTERMINAL_ENUM_SIZE) {//checks it the curr location in the rhs is a terminal
                    updateSet(first[(*it_g).lhs], (tokens) (*it), &finished);//for X->aY : first(X)<-first(X) U a
                    break;
                }
                insert_set(first[(*it_g).lhs], first[*it], &finished); //for X->AB : first(X)<-first(X) U first(A)
                if (!is_nullable((nonterminal(*it)))) {//checks if the current nonterminal of the rhs is nullable
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
void compute_first() {
    std::vector<std::set<tokens> > first;
    compute_vector_first(first);
    print_first(first);
}

//computes vector of all follow sets
void compute_vector_follow(std::vector<std::set<tokens> > &follow) {
    std::vector<std::set<tokens> > first;
    compute_vector_first(first);
    //initiate follow to contain NONTERMINAL_ENUM_SIZE empty sets
    for (int i = 0; i < NONTERMINAL_ENUM_SIZE; i++) {
        follow.push_back(std::set<tokens>());
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
                            if (*(it_copy) > NONTERMINAL_ENUM_SIZE) { // if we got to a terminal
                                updateSet(follow[i], (tokens) (*(it_copy)),
                                          &finished); // insert the terminal to the follow
                                break;
                            } else {
                                insert_set(follow[i], first[*(it_copy)], &finished);
                                if (!is_nullable(
                                        (nonterminal) (*(it_copy)))) { // if the current nonterminal is not nullable
                                    break;
                                }
                            }
                            ++it_copy;
                        }
                        if (it_copy == (*it_g).rhs.end()) { // if all the rest of the rhs is nullable
                            insert_set(follow[i], follow[(*it_g).lhs],
                                       &finished); // add the follow of the current rule lhs
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


//return a vetor of all the select sets for each rule in the grammar
void compute_vector_select(std::vector<std::set<tokens> > &select) {
    std::vector<std::set<tokens> > follow;
    compute_vector_follow(follow);
    std::vector<std::set<tokens> > first;
    compute_vector_first(first);

    std::set<tokens> curr_rule_sel = std::set<tokens>();

    bool finish = false; //not really necessary
    bool rhs_nullable = true;
    //going over all the rules
    for (std::vector<grammar_rule>::iterator it_g = grammar.begin(); it_g != grammar.end(); ++it_g) {
        curr_rule_sel = std::set<tokens>();
        rhs_nullable = true;
        for (std::vector<int>::iterator it = (*it_g).rhs.begin();
             it != (*it_g).rhs.end(); ++it) { //going over the rhs of a rule
            if (*it > NONTERMINAL_ENUM_SIZE) { // if the current varible is token
                updateSet(curr_rule_sel, (tokens) (*(it)),
                          &finish); // insert the token to the select of the current rule
                rhs_nullable = false;
                break;
            } else {
                insert_set(curr_rule_sel, first[*it],
                           &finish); //  insert the nonterminal to the select of the current rule
                if (!is_nullable((nonterminal) *it)) { // if the nonterminal is not nullable
                    rhs_nullable = false;
                    break;
                }
            }
        }
        if (rhs_nullable) {
            insert_set(curr_rule_sel, follow[(*it_g).lhs], &finish);
        }
        select.push_back(curr_rule_sel);
    }
}

/**
 * computes select for all grammar rules (see grammar global variable in grammar.h)
 * calls print_select when finished
 */
void compute_select() {
    std::vector<std::set<tokens> > select;
    compute_vector_select(select);
    print_select(select);
}

int select_for_single_rule(nonterminal X, tokens t) {
    std::vector<std::set<tokens> > select;
    compute_vector_select(select);
    int num_rule = 0;
    for (std::vector<grammar_rule>::iterator it_g = grammar.begin(); it_g != grammar.end(); ++it_g) {
        if ((((*it_g).lhs) == X) && (((select[num_rule]).find(t)) != (select[num_rule]).end())) {
            return num_rule+1;
        }
        num_rule++;
    }
    return -1;
}

std::vector<tokens> allTokens() {
    std::vector<tokens> T = std::vector<tokens>();

    for (int cur = (int) KEY; cur <= (int) EF; cur++) {
        T.push_back((tokens) cur);
    }

    return T;
}

std::vector<nonterminal> allNonTerminals() {
    std::vector<nonterminal> V = std::vector<nonterminal>();

    for (int cur = (int) S; cur < (int) NONTERMINAL_ENUM_SIZE; cur++) {
        V.push_back((nonterminal) cur);
    }

    return V;
}


/**
 * implements an LL(1) parser for the grammar using yylex()
 */
void parser() {
    //the stack Q and the table M
    std::vector<int> Q = std::vector<int>();
    std::map<nonterminal, std::map<tokens, int> > M = std::map<nonterminal, std::map<tokens, int> >();

    //vectors of terminals and nonterminals for convinience
    std::vector<tokens> T = allTokens();
    std::vector<nonterminal> V = allNonTerminals();

    //initialize the map M
    //for each nonterminal
    for (int v_num = 0; v_num < V.size(); v_num++) {
        //for each terminal
        for (int t_num = 0; t_num < T.size(); t_num++) {
            //for each rule in the grammar that starts with X
            M[V[v_num]][T[t_num]] = select_for_single_rule(V[v_num], T[t_num]);
        }
    }

    //init token and queue
    Q.push_back(S);
    tokens t = (tokens) yylex();

    //while the stack is not empty
    while (!Q.empty()) {
        int res = Q.back();
        //if X is terminal
        if (res > NONTERMINAL_ENUM_SIZE) {
            //if the terminal is the one we need, move to the next one
            if ((tokens) res == t) {
                t = (tokens) yylex();
                Q.pop_back();
            } else {
                //otherwise we have an error and should break
                break;
            }
        } else {//X is nonterminal
            nonterminal X = (nonterminal) res;
//            std::cout << M[X][t] << std::endl;
            //if there is a rule for these X and t
            if (M[X][t] != -1) {
                //pop X out of the stack
                Q.pop_back();
                //print the number of the rule we are using
                std::cout << M[X][t] << std::endl;
                //make a copy of the right side of the rule
                std::vector<int> rhs = std::vector<int>(grammar[M[X][t] - 1].rhs);



                while (!rhs.empty()) {
//                    std::cout << rhs.back() << std::endl;
                    Q.push_back(rhs.back());
                    rhs.pop_back();
                }

                //add all of the stuff in the rule in reverse order
//                if(rhs[0] == 1){
//                    Q.push_back(rhs[1]);
//                } else{
//
//                    for(int* cur_part = (int*)rhs[1]; cur_part >= (int*)rhs[0]; cur_part--){
//                        Q.push_back(*cur_part);
//                    }
//                }




//                if(rhs[0] != 1){
//                    while(!rhs.empty()){
//                        Q.push_back(rhs.back());
//                        rhs.pop_back();
//                    }
//                } else {
//                    Q.push_back(rhs[1]);
//                }

//                std::cout << Q.back() << std::endl;
            } else {
                //otherwise we have an error and should break
                break;
            }
        }
    }

    //if the queue is not empty or if the last terminal isn't EOF, it's an error
    if (!Q.empty() || t != EF) {
        std::cout << "Syntax error" << std::endl;
        return;
    }

    //if we got here, we had a success!
    std::cout << "Success" << std::endl;
}


