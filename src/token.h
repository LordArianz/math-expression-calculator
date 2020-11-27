//
// Created by S.Morteza Ezzabady on 10/24/20.
//

#ifndef CALCULATOR_TOKEN_H
#define CALCULATOR_TOKEN_H

#include <iostream>
#include <string>
#include <map>
#include <vector>

class Token {
public:
    Token(std::string str);
    virtual double eval() = 0;
    virtual void print() = 0;
    std::string getRaw();

    static const std::string numReg;
    static const std::string asgReg;
    static const std::string optReg;
    static const std::string varReg;
    static const std::string phdReg;

protected:
    std::string rawValue;
};

class NumToken: public Token {
public:
    NumToken(std::string str);
    double eval();
    void print();
};

class OptToken: public Token {
public:
    OptToken(std::string str);
    double eval();
    void print();
};

class VarToken: public Token {
    std::map<std::string, double>* varsMap;
    double asg(double x);
    double add(double x);
    double sub(double x);
    double mul(double x);
    double div(double x);
    double mod(double x);
public:
    VarToken(std::string str, std::map<std::string, double>* vars);
    double eval();
    void print();
    double set(double x, OptToken* opt);
};
#endif //CALCULATOR_TOKEN_H
