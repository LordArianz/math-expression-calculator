//
// Created by S.Morteza Ezzabady on 10/20/20.
//

#include "expression.h"

Expression::Expression(std::string str, std::map<std::string, double> *vars,
                       std::map<std::string, double(*)(double, ...)> *funcs) {
    tokens = tokensFromString(str, vars, funcs);
}

double Expression::eval() {
    std::queue<Token *> rpnQueue = rpn();
    std::stack<double> evalStack;
    VarToken *v;
    while (!rpnQueue.empty()) {
        Token *token = rpnQueue.front();
        rpnQueue.pop();
        NumToken *num = dynamic_cast<NumToken *>(token);
        OptToken *opt = dynamic_cast<OptToken *>(token);
        VarToken *var = dynamic_cast<VarToken *>(token);
        FunToken *fun = dynamic_cast<FunToken *>(token);

        if (num) {
            evalStack.push(num->eval());
        } else if (opt) {
            if (evalStack.size() < 2) {
                if (evalStack.size() == 1 && v && opt->getRaw().back() == '=') {
                    double right = evalStack.top();
                    evalStack.pop();
                    evalStack.push(v->set(right, opt));
                    continue;
                } else
                    throw std::domain_error("Invalid Equation");
            }
            double right = evalStack.top();
            evalStack.pop();
            double left = evalStack.top();
            evalStack.pop();
            std::string optStr = opt->getRaw();
            if (optStr == "+") {
                evalStack.push(left + right);
            } else if (optStr == "-") {
                evalStack.push(left - right);
            } else if (optStr == "*") {
                evalStack.push(left * right);
            } else if (optStr == "/") {
                evalStack.push(left / right);
            } else if (optStr == "%") {
                evalStack.push((int) left % (int) right);
            } else if (optStr == "^") {
                evalStack.push(pow(left, right));
            }
        } else if (var) {
            v = var;
        } else if (fun) {
            if(evalStack.size() < fun->getArgCount()) {
                throw std::domain_error("Invalid Equation");
            }
            std::vector<double> args;
            for(int i=0;i<fun->getArgCount();i++){
                args.push_back(evalStack.top());
                evalStack.pop();
            }
            fun->setArgs(&args);
            evalStack.push(fun->eval());
        }
    }
    if (evalStack.size() != 1)
        throw std::domain_error("Invalid Equation");
    return answer = evalStack.top();
}

void Expression::print() {
    std::cout << "answer -> " << answer << '\n';
}

std::vector<Token *> Expression::tokensFromString(std::string str, std::map<std::string, double> *vars,
                                                  std::map<std::string, double(*)(double, ...)> *funcs) {
    std::string asg = "(" + Token::varReg + ")\\s*(" + Token::asgReg + ")";
    std::string fun = "(" + Token::varReg + ")\\s*(\\()";
    std::string equ = "(" + fun + ")|(" + Token::varReg + ")|(" + Token::optReg + ")|(" + Token::numReg + ")";
    std::regex rgx("^(" + asg + ")?|(" + equ + ")+|(\\;?)$");
    std::sregex_token_iterator missIter(str.begin(), str.end(), rgx, -1);
    std::sregex_token_iterator end;
    for (; missIter != end; ++missIter) {
        std::string r = *missIter;
        r.erase(std::remove_if(r.begin(), r.end(), isspace), r.end());
        if (!r.empty()) {
            throw std::domain_error("Invalid Token");
        }
    }
    std::vector<Token *> tokens;
    std::sregex_token_iterator iter(str.begin(), str.end(), rgx, 0);
    for (; iter != end; ++iter) {
        std::string r = *iter;
        std::string innerRgxStr;
        bool isAsg = false;
        if (std::regex_match(r.begin(), r.end(), std::regex(asg))) {
            innerRgxStr = "(" + Token::varReg + ")|(" + Token::asgReg + ")";
            isAsg = true;
        } else if (std::regex_match(r.begin(), r.end(), std::regex("(" + equ + ")+"))) {
            innerRgxStr = equ;
        } else {
            continue;
        }
        std::regex innerRgx(innerRgxStr);
        std::sregex_token_iterator innerIter(r.begin(), r.end(), innerRgx, 0);
        for (; innerIter != end; ++innerIter) {
            std::string q = *innerIter;
            if (std::regex_match(q.begin(), q.end(), std::regex(fun))) {
                q.pop_back();
                if(!funcs->count(q)) {
                    throw std::domain_error("Invalid Function " + q);
                }
                tokens.push_back(new FunToken(q, funcs));
                tokens.push_back(new OptToken("("));
            } else if (std::regex_match(q.begin(), q.end(), std::regex(Token::optReg)) ||
                       std::regex_match(q.begin(), q.end(), std::regex(Token::asgReg))) {
                tokens.push_back(new OptToken(q));
            } else if (std::regex_match(q.begin(), q.end(), std::regex(Token::numReg))) {
                tokens.push_back(new NumToken(q));
            } else if (std::regex_match(q.begin(), q.end(), std::regex(Token::varReg))) {
                if (!isAsg) {
                    if (!vars->count(q)) {
                        throw std::domain_error("Invalid Variable " + q);
                    }
                    tokens.push_back(new NumToken(std::to_string(vars->find(q)->second)));
                } else
                    tokens.push_back(new VarToken(q, vars));
            }
        }
    }
//    for (Token *t: tokens)
//        t->print();
    return tokens;
}

std::queue<Token *> Expression::rpn() {
    std::queue<Token *> rpnQueue;
    std::stack<Token *> optStack;
    std::stack<bool> wereValue;
    std::stack<int> argCount;
    bool lastWasOp = true;
    for (int i = 0; i < tokens.size(); i++) {
        NumToken *num = dynamic_cast<NumToken *>(tokens[i]);
        OptToken *opt = dynamic_cast<OptToken *>(tokens[i]);
        VarToken *var = dynamic_cast<VarToken *>(tokens[i]);
        FunToken *fun = dynamic_cast<FunToken *>(tokens[i]);

        if (num) {
            rpnQueue.push(tokens[i]);
            lastWasOp = false;
            if (!wereValue.empty()) {
                wereValue.pop();
                wereValue.push(true);
            }
        } else if (opt) {
            if (opt->getRaw() == "(")
                optStack.push(opt);
            else if (opt->getRaw() == ")") {
                while (!optStack.empty() &&
                       (dynamic_cast<FunToken *>(optStack.top()) || ((OptToken *) optStack.top())->getRaw() != "(")) {
                    rpnQueue.push(optStack.top());
                    optStack.pop();
                }
                if (optStack.empty())
                    throw std::domain_error("Invalid Equation");
                optStack.pop();
                if (!optStack.empty() && dynamic_cast<FunToken *>(optStack.top())) {
                    FunToken *top = (FunToken *) optStack.top();
                    optStack.pop();
                    int cnt = argCount.top();
                    argCount.pop();
                    bool w = wereValue.top();
                    wereValue.pop();
                    if (w)
                        cnt++;
                    top->setArgCount(cnt);
                    rpnQueue.push(top);
                }
            } else if (opt->getRaw() == ",") {
                bool miss = true;
                while (true) {
                    if (optStack.empty())
                        break;
                    Token *top = optStack.top();
                    if (dynamic_cast<FunToken *>(top)) {
                        rpnQueue.push(top);
                        optStack.pop();
                        continue;
                    }
                    if (((OptToken *) top)->getRaw() == "(") {
                        miss = false;
                        break;
                    }
                    rpnQueue.push(top);
                    optStack.pop();
                }
                if (miss) {
                    throw std::domain_error("Miss");
                }
                bool w = wereValue.top();
                wereValue.pop();
                if (w) {
                    int cnt = argCount.top();
                    argCount.pop();
                    argCount.push(cnt + 1);
                    wereValue.push(false);
                }
            } else {
                if (lastWasOp) {
                    if (opt->getRaw() == "-" || opt->getRaw() == "+") {
                        rpnQueue.push(new NumToken("0"));
                    } else {
                        throw std::domain_error("Invalid Operator");
                    }
                }
                while (!optStack.empty() && opt->eval() <= optStack.top()->eval()) {
                    rpnQueue.push(optStack.top());
                    optStack.pop();
                }
                optStack.push(opt);
            }
        } else if (var) {
            rpnQueue.push(tokens[i]);
            lastWasOp = false;
        } else if (fun) {
            optStack.push(fun);
            argCount.push(0);
            if (!wereValue.empty()) {
                wereValue.pop();
                wereValue.push(true);
            }
            wereValue.push(false);
        }
    }
    while (!optStack.empty()) {
        rpnQueue.push(optStack.top());
        optStack.pop();
    }
    return rpnQueue;
}
