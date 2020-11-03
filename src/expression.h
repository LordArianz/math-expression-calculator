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
#include "token.h"

class Expression {
    std::string rawValue;
    double answer;
    std::vector<Token*> tokens;
public:
    Expression(std::string str, std::map<std::string, double>* vars = 0);

    double eval();

    void print();

    std::vector<Token*> tokensFromString(std::string str, std::map<std::string, double> *vars);

    std::queue<Token*> rpn();
};


#endif //CALCULATOR_EXPRESSION_H
