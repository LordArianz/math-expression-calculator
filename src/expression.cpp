//
// Created by S.Morteza Ezzabady on 10/20/20.
//

#include "expression.h"

Expression::Expression(std::string str, std::map<std::string, double> *vars) {
    tokens = tokensFromString(str, vars);
}

double Expression::eval() {
    std::queue<Token*> rpnQueue = rpn();
    std::stack<double> evalStack;
    VarToken* v;
    while(!rpnQueue.empty()) {
        Token* token = rpnQueue.front();
        rpnQueue.pop();
        NumToken* num = dynamic_cast<NumToken*>(token);
        OptToken* opt = dynamic_cast<OptToken*>(token);
        VarToken* var = dynamic_cast<VarToken*>(token);
        if(num) {
            evalStack.push(num->eval());
        } else if(opt) {
            if(evalStack.size() < 2) {
                if(evalStack.size() == 1 && v && opt->getRaw().back() == '=') {
                    double right = evalStack.top();
                    evalStack.pop();
                    if(opt->getRaw().front() == '=') {
                        evalStack.push(v->set(right));
                    } else if(opt->getRaw().front() == '+') {
                        evalStack.push(v->add(right));
                    } else if(opt->getRaw().front() == '-') {
                        evalStack.push(v->sub(right));
                    } else if(opt->getRaw().front() == '*') {
                        evalStack.push(v->mul(right));
                    } else if(opt->getRaw().front() == '/') {
                        evalStack.push(v->div(right));
                    } else if(opt->getRaw().front() == '%') {
                        evalStack.push(v->mod(right));
                    }
                    continue;
                } else
                    throw std::domain_error("Invalid equation");
            }
            double right = evalStack.top();
            evalStack.pop();
            double left  = evalStack.top();
            evalStack.pop();
            std::string optStr = opt->getRaw();
            if(optStr == "+") {
                evalStack.push(left + right);
            } else if(optStr == "-") {
                evalStack.push(left - right);
            } else if(optStr == "*") {
                evalStack.push(left * right);
            } else if(optStr == "/") {
                evalStack.push(left / right);
            } else if(optStr == "%") {
                evalStack.push((int)left % (int)right);
            } else if(optStr == "^") {
                evalStack.push(pow(left, right));
            }
        } else if(var) {
            v = var;
        }
    }
    return answer = evalStack.top();
}

void Expression::print() {
    std::cout << "answer -> " << answer << '\n';
}

std::vector<Token *> Expression::tokensFromString(std::string str, std::map<std::string, double> *vars) {
    std::string asg = "(" + Token::varReg + ")\\s*(" + Token::asgReg + ")";
    std::string equ = "(" + Token::varReg + ")|(" + Token::optReg + ")|(" + Token::numReg + ")";
    std::regex rgx("^(" + asg + ")?|(" + equ + ")+|(\\;?)$");
    std::sregex_token_iterator missIter(str.begin(), str.end(), rgx, -1);
    std::sregex_token_iterator end;
    for (; missIter != end; ++missIter) {
        std::string r = *missIter;
        r.erase(std::remove_if(r.begin(), r.end(), isspace), r.end());
        if(!r.empty()) {
            throw std::domain_error("Invalid Token");
        }
    }
    std::vector<Token *> tokens;
    std::sregex_token_iterator iter(str.begin(), str.end(), rgx, 0);
    for(; iter != end; ++iter) {
        std::string r = *iter;
        std::string innerRgxStr;
        bool isAsg = false;
        if(std::regex_match(r.begin(), r.end(), std::regex(asg))) {
            innerRgxStr = "(" + Token::varReg + ")|(" + Token::asgReg + ")";
            isAsg = true;
        } else if(std::regex_match(r.begin(), r.end(), std::regex("(" + equ + ")+"))) {
            innerRgxStr = equ;
        } else {
            continue;
        }
        std::regex innerRgx(innerRgxStr);
        std::sregex_token_iterator innerIter(r.begin(), r.end(), innerRgx, 0);
        for(; innerIter != end; ++innerIter) {
            std::string q = *innerIter;
            if(std::regex_match(q.begin(), q.end(), std::regex(Token::varReg))) {
                if(!isAsg) {
                    if(!vars->count(q))
                        throw std::domain_error("Invalid Variable");
                    tokens.push_back(new NumToken(std::to_string(vars->find(q)->second)));
                }
                else
                    tokens.push_back(new VarToken(q, vars));
            }
            else if(std::regex_match(q.begin(), q.end(), std::regex(Token::optReg)) ||
                    std::regex_match(q.begin(), q.end(), std::regex(Token::asgReg))) {
                tokens.push_back(new OptToken(q));
            }
            else if(std::regex_match(q.begin(), q.end(), std::regex(Token::numReg))) {
                tokens.push_back(new NumToken(q));
            }
        }
    }
    return tokens;
}

std::queue<Token *> Expression::rpn() {
    std::queue<Token*> rpnQueue;
    std::stack<Token*> optStack;
    bool lastWasOp = true;
    for(int i = 0; i < tokens.size(); i++){
        NumToken* num = dynamic_cast<NumToken*>(tokens[i]);
        OptToken* opt = dynamic_cast<OptToken*>(tokens[i]);
        VarToken* var = dynamic_cast<VarToken*>(tokens[i]);

        if(num) {
            rpnQueue.push(tokens[i]);
            lastWasOp = false;
        } else if(opt) {
            if(opt->getRaw() == "(")
                optStack.push(opt);
            else if(opt->getRaw() == ")") {
                while(((OptToken*)optStack.top())->getRaw() != "(") {
                    rpnQueue.push(optStack.top());
                    optStack.pop();
                }
                optStack.pop();
            } else {
                if(lastWasOp) {
                    if(opt->getRaw() == "-" || opt->getRaw() == "+") {
                        rpnQueue.push(new NumToken("0"));
                    } else {
                        throw std::domain_error("Invalid Operator");
                    }
                }
                while(!optStack.empty() && opt->eval() <= optStack.top()->eval()) {
                    rpnQueue.push(optStack.top());
                    optStack.pop();
                }
                optStack.push(opt);
            }
        } else if(var) {
            rpnQueue.push(tokens[i]);
            lastWasOp = false;
        }
    }
    while(!optStack.empty()) {
        rpnQueue.push(optStack.top());
        optStack.pop();
    }
    return rpnQueue;
}
