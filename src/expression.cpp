//
// Created by S.Morteza Ezzabady on 10/20/20.
//

#include "expression.h"

Expression::Expression(std::string str,
                       std::map<std::string, double> *vars,
                       std::map<std::string, std::pair<double(*)(double, ...), int> > *funcs,
                       std::map<std::string, std::pair<std::deque<Token *>*, int> > *specials):
                       funcs{funcs}, specials{specials} {
    rawValue = str;
    tokens = tokensFromString(str, vars, funcs, specials);
}

double Expression::eval() {
    std::deque<Token *>* rpnQueue = rpn();
    int ulCnt = 0;
    for(int i=0;i<rpnQueue->size();i++) {
        if(rpnQueue->at(i)->getRaw()[0] == '_'){
            if(!(dynamic_cast<VarToken *>(rpnQueue->front()) && rpnQueue->back()->getRaw() == "="))
                throw std::domain_error("Invalid Equation");
            ulCnt++;
        }
    }
    if(ulCnt){
        std::string newFunc = rpnQueue->front()->getRaw();
        rpnQueue->pop_front();
        rpnQueue->pop_back();
        (*specials)[newFunc] = std::make_pair(rpnQueue, ulCnt);
        return 0;
    }
    std::stack<double> evalStack;
    VarToken *v;
    while (!rpnQueue->empty()) {
        Token *token = rpnQueue->front();
        rpnQueue->pop_front();
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

std::vector<Token *> Expression::tokensFromString(std::string str,
                                                  std::map<std::string, double> *vars,
                                                  std::map<std::string, std::pair<double(*)(double, ...), int> > *funcs,
                                                  std::map<std::string, std::pair<std::deque<Token *>*, int> > *specials) {
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
                if(!funcs->count(q) && !specials->count(q)) {
                    throw std::domain_error("Invalid Function " + q);
                }
                tokens.push_back(new FunToken(q, funcs, specials));
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

int next(std::set<int> s){ // need to be better
    int x = 1;
    for(int y: s){
        if(y > x)
            return x;
        x++;
    }
    return x;
}

std::deque<Token *>* Expression::rpn() {
    int ulCnt = 0;
    int phCnt = 0;
    for (int i = 0; i < rawValue.size(); i++)
        if (rawValue[i] == '_')
            ulCnt++;
    std::deque<Token *> *rpnQueue = new std::deque<Token *>();
    std::stack<Token *> optStack;
    std::stack<bool> wereValue;
    std::stack<int> argCount;
    std::set<int> phdSet;
    std::vector<int> phdVec;

    bool lastWasOp = true;
    for (int i = 0; i < tokens.size(); i++) {
        NumToken *num = dynamic_cast<NumToken *>(tokens[i]);
        OptToken *opt = dynamic_cast<OptToken *>(tokens[i]);
        VarToken *var = dynamic_cast<VarToken *>(tokens[i]);
        FunToken *fun = dynamic_cast<FunToken *>(tokens[i]);

        if (num) {
            rpnQueue->push_back(tokens[i]);
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
                    rpnQueue->push_back(optStack.top());
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
                    if (w) {
                        cnt++;
                        std::string raw = rpnQueue->back()->getRaw();
                        if (std::regex_match(raw.begin(), raw.end(), std::regex(Token::phdReg))) {
                            phCnt++;
                            int p = std::atoi(raw.substr(1, raw.size() - 1).c_str());
                            phdSet.insert(p);
                            phdVec.push_back(p);
                            rpnQueue->pop_back();
                            rpnQueue->push_back(new PhdToken(raw));
                        }
                    }
                    top->setArgCount(cnt);
                    top->placeholders = std::vector<int>(phdVec);
                    phdVec.clear();
                    rpnQueue->push_back(top);
                }
            } else if (opt->getRaw() == ",") {
                bool miss = true;
                while (true) {
                    if (optStack.empty())
                        break;
                    Token *top = optStack.top();
                    if (dynamic_cast<FunToken *>(top)) {
                        rpnQueue->push_back(top);
                        optStack.pop();
                        continue;
                    }
                    if (((OptToken *) top)->getRaw() == "(") {
                        miss = false;
                        break;
                    }
                    rpnQueue->push_back(top);
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
                    std::string raw = rpnQueue->back()->getRaw();
                    if (std::regex_match(raw.begin(), raw.end(), std::regex(Token::phdReg))) {
                        phCnt++;
                        int p = std::atoi(raw.substr(1, raw.size() - 1).c_str());
                        phdSet.insert(p);
                        phdVec.push_back(p);
                        rpnQueue->pop_back();
                        rpnQueue->push_back(new PhdToken(raw));
                    }
                }
            } else {
                if (lastWasOp) {
                    if (opt->getRaw() == "-" || opt->getRaw() == "+") {
                        rpnQueue->push_back(new NumToken("0"));
                    } else {
                        throw std::domain_error("Invalid Operator");
                    }
                }
                while (!optStack.empty() && opt->eval() <= optStack.top()->eval()) {
                    rpnQueue->push_back(optStack.top());
                    optStack.pop();
                }
                optStack.push(opt);
            }
        } else if (var) {
            rpnQueue->push_back(tokens[i]);
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
    if (phCnt != ulCnt) {
        throw std::domain_error("Invalid Equation");
    }
    while (!optStack.empty()) {
        rpnQueue->push_back(optStack.top());
        optStack.pop();
    }
    for(int i=0;i<rpnQueue->size();i++){
        FunToken *fun = dynamic_cast<FunToken *>(rpnQueue->at(i));
        if(fun){
            if(fun->getNeededArgCount() > fun->getArgCount()){
                int n = next(phdSet);
                phdSet.insert(n);
                rpnQueue->insert(rpnQueue->begin() + i, new PhdToken('_' + std::to_string(n)));
                fun->setArgCount(fun->getArgCount() + 1);
            }
        }
    }
//    for(int i=0;i<rpnQueue->size();i++) {
//        rpnQueue->at(i)->print();
//    }
    return rpnQueue;
}
