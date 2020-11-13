//
// Created by S.Morteza Ezzabady on 11/8/20.
//

#ifndef CALCULATOR_FUN_TOKEN_H
#define CALCULATOR_FUN_TOKEN_H

#include <iostream>
#include "token.h"
#include "cmath"

class FunToken: public Token {
    std::vector<double>* args;
    int argCount;
    std::map<std::string, double(*)(double, ...)> *funcs;
public:
    FunToken(std::string str, std::map<std::string, double(*)(double, ...)> *funcs);
    double eval();
    void print();
    void setArgCount(int cnt);
    int getArgCount();
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

};


#endif //CALCULATOR_FUN_TOKEN_H
