//
// Created by S.Morteza Ezzabady on 11/8/20.
//

#ifndef CALCULATOR_FUN_TOKEN_H
#define CALCULATOR_FUN_TOKEN_H

#include <iostream>
#include <deque>
#include <stack>
#include "token.h"
#include "cmath"

class PhdToken: public Token {
public:
    PhdToken(std::string str);
    double eval();
    void print();
};

class FunToken: public Token {
    std::vector<double>* args;
    int argCount;
    std::map<std::string, std::pair<double(*)(double, ...), int> > *funcs;
    std::map<std::string, std::pair<std::deque<Token *>*, int> > *specials;
public:
    FunToken(std::string str,
             std::map<std::string, std::pair<double(*)(double, ...), int> > *funcs,
             std::map<std::string, std::pair<std::deque<Token *>*, int> > *specials);
    double eval();
    double specialEval();
    void print();
    void setArgCount(int cnt);
    int getArgCount();
    int getNeededArgCount();
    void setArgs(std::vector<double>* args);
    static double SIN(double t, ...);
    static double COS(double t, ...);
    static double TAN(double t, ...);
    static double LOG(double t, ...);
    static double EXP(double t, ...);
    static double SQRT(double t, ...);
    static double POW(double t, ...);
    static double HYPOT(double t, ...);
    static double LERP(double t, ...);
    static double polynomial(double t, ...);
    std::vector<int> placeholders;
};


#endif //CALCULATOR_FUN_TOKEN_H
