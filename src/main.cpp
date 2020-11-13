#include <iostream>
#include <map>
#include <fstream>
#include "expression.h"
#include <functional>

std::ifstream in;
std::map<std::string, double> vars;
std::map<std::string, double(*)(double, ...)> funcs;
std::string input;


int main(int argc, char *argv[]) {
    funcs["sin"] = &FunToken::SIN;
    funcs["cos"] = &FunToken::COS;
    funcs["tan"] = &FunToken::TAN;
    funcs["log"] = &FunToken::LOG;
    funcs["exp"] = &FunToken::EXP;
    funcs["sqrt"] = &FunToken::SQRT;
    funcs["pow"] = &FunToken::POW;
    funcs["hypot"] = &FunToken::HYPOT;
    funcs["lerp"] = &FunToken::LERP;
    funcs["polynomial"] = &FunToken::polynomial;
    if(argc == 2) {
        in.open(argv[1]);
        if(in.is_open()){
            std::cin.rdbuf(in.rdbuf());
        }
    }
    while(true) {
        if(!std::getline(std::cin, input))
            break;
        try {
            Expression exp(input, &vars, &funcs);
            exp.eval();
            if(input.back() != ';')
                exp.print();
        } catch (std::domain_error &e) {
            std::cout << e.what() << '\n';
        }
    }
    for(auto item: vars)
        std::cout << item.first << " = " << item.second << '\n';
    return 0;
}
