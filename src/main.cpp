#include <iostream>
#include <map>
#include <fstream>
#include "expression.h"
#include <functional>

std::ifstream in;
std::map<std::string, double> vars;
std::map<std::string, std::pair<double(*)(double, ...), int> > funcs;
std::map<std::string, std::pair<std::deque<Token *>*, int> > specials;
std::string input;


int main(int argc, char *argv[]) {
    funcs["sin"] = std::make_pair(&FunToken::SIN, 1);
    funcs["cos"] = std::make_pair(&FunToken::COS, 1);
    funcs["tan"] = std::make_pair(&FunToken::TAN, 1);
    funcs["log"] = std::make_pair(&FunToken::LOG, 1);
    funcs["exp"] = std::make_pair(&FunToken::EXP, 1);
    funcs["sqrt"] = std::make_pair(&FunToken::SQRT, 1);
    funcs["pow"] = std::make_pair(&FunToken::POW, 2);
    funcs["hypot"] = std::make_pair(&FunToken::HYPOT, 2);
    funcs["lerp"] = std::make_pair(&FunToken::LERP, 3);
    funcs["polynomial"] = std::make_pair(&FunToken::polynomial, 0);
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
            Expression exp(input, &vars, &funcs, &specials);
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
