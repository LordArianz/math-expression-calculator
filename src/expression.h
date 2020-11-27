//
// Created by S.Morteza Ezzabady on 10/20/20.
//

#ifndef CALCULATOR_EXPRESSION_H
#define CALCULATOR_EXPRESSION_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <queue>
#include <stack>
#include <cmath>
#include <set>
#include <deque>
#include "token.h"
#include "fun_token.h"

class Expression {
    std::string rawValue;
    double answer;
    std::vector<Token*> tokens;
    std::map<std::string, std::pair<double(*)(double, ...), int> > *funcs;
    std::map<std::string, std::pair<std::deque<Token *>*, int> > *specials;
public:
    Expression(std::string str,
               std::map<std::string, double>* vars = 0,
               std::map<std::string, std::pair<double(*)(double, ...), int> > *funcs = 0,
               std::map<std::string, std::pair<std::deque<Token *>*, int> > *specials = 0);

    double eval();

    void print();

    std::vector<Token*> tokensFromString(std::string str,
                                         std::map<std::string, double> *vars,
                                         std::map<std::string, std::pair<double(*)(double, ...), int> > *funcs,
                                         std::map<std::string, std::pair<std::deque<Token *>*, int> > *specials);

    std::deque<Token*>* rpn();
};


#endif //CALCULATOR_EXPRESSION_H
