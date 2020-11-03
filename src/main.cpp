#include <iostream>
#include <map>
#include <fstream>
#include "expression.h"

std::ifstream in;
std::map<std::string, double> vars;
std::string input;

int main(int argc, char *argv[]) {
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
            Expression exp(input, &vars);
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
