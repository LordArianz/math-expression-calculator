//
// Created by S.Morteza Ezzabady on 10/24/20.
//

#include "token.h"

const std::string Token::numReg = "(([0-9]*[.])?[0-9]+)|(_[0-9]+)";
const std::string Token::asgReg = R"((=)|(\+=)|(\-=)|(\*=)|(\/=)|(\%=))";
const std::string Token::optReg = R"((\+)|(\-)|(\*)|(\/)|(\%)|(\^)|(\()|(\))|(\,))";
const std::string Token::varReg = "([a-z]|[A-Z])([a-z]|[A-Z]|[0-9])*";
const std::string Token::phdReg = "(_[0-9]+)";

Token::Token(std::string str) : rawValue{str} {}

double NumToken::eval() {
    return std::atof(rawValue.c_str());
}

NumToken::NumToken(std::string str) : Token(str) {}

void NumToken::print() {
    std::cout << "number " << rawValue << '\n';
}

double OptToken::eval() {
    if(rawValue == "(")
        return 0;
    if(rawValue == "+" || rawValue == "-")
        return 1;
    if(rawValue == "*" || rawValue == "/" || rawValue == "%")
        return 2;
    if(rawValue == "^")
        return 3;
    return -1;
}

OptToken::OptToken(std::string str) : Token(str) {}

void OptToken::print() {
    std::cout << "operator " << rawValue << '\n';
}

std::string Token::getRaw() {
    return rawValue;
}

double VarToken::eval() {
    if(!varsMap->count(rawValue))
        return INT_MIN;
    return varsMap->find(rawValue)->second;
}

VarToken::VarToken(std::string str, std::map<std::string, double> *vars) : Token(str), varsMap{vars} {}

void VarToken::print() {
    std::cout << "variable " << rawValue << " = " << eval() << '\n';
}

double VarToken::asg(double x) {
    return (*varsMap)[rawValue] = x;
}

double VarToken::add(double x) {
    return (*varsMap)[rawValue] += x;
}

double VarToken::sub(double x) {
    return (*varsMap)[rawValue] -= x;
}

double VarToken::mul(double x) {
    return (*varsMap)[rawValue] *= x;
}

double VarToken::div(double x) {
    return (*varsMap)[rawValue] /= x;
}

double VarToken::mod(double x) {
    return (*varsMap)[rawValue] = (int)(*varsMap)[rawValue] % (int)x;
}

double VarToken::set(double x, OptToken *opt) {
    if(opt->getRaw().front() == '=')
        return asg(x);
    if(opt->getRaw().front() == '+')
        return add(x);
    if(opt->getRaw().front() == '-')
        return sub(x);
    if(opt->getRaw().front() == '*')
        return mul(x);
    if(opt->getRaw().front() == '/')
        return div(x);
    if(opt->getRaw().front() == '%')
        return mod(x);
    return eval();
}
