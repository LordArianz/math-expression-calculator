//
// Created by S.Morteza Ezzabady on 11/8/20.
//

#include "fun_token.h"

#define PI 3.14159265359

double PhdToken::eval() {
    return std::atof(rawValue.substr(1, rawValue.size() - 1).c_str());
}

PhdToken::PhdToken(std::string str) : Token(str) {}

void PhdToken::print() {
    std::cout << "placeholder " << rawValue << '\n';
}

template <class F, class T, std::size_t N, std::size_t... Idx>
decltype(auto) apply_impl (F f, T (&t)[N], std::index_sequence<Idx...>) {
    return f(t[Idx]...);
}

template <class F, class T, std::size_t N>
decltype(auto) apply (F f, T (&t)[N]) {
    return apply_impl(f, t, std::make_index_sequence<N>{});
}

double FunToken::eval() { // need change :(
    double a[100];
    a[0] = argCount;
    for(int i = 0; i < args->size(); i++)
        a[i + 1] = args->at(args->size() - i - 1);
    double ans = 0;
    if(funcs->count(rawValue))
        ans = apply([this](auto...xs){return (*(*(funcs))[rawValue].first)(std::forward<decltype(xs)>(xs)...);}, a);
    else if(specials->count(rawValue))
        ans = specialEval();
    return ans;
}

FunToken::FunToken(std::string str,
                   std::map<std::string, std::pair<double(*)(double, ...), int> > *funcs,
                   std::map<std::string, std::pair<std::deque<Token *>*, int> > *specials) :
                   Token(str), funcs{funcs}, argCount{0}, specials{specials} {
                       placeholders = std::vector<int>();
                   }

void FunToken::print() {
    std::cout << "function " << rawValue << ' ' << getNeededArgCount() << "( ";
//    for(int i = args->size() - 1; i >= 0; i--){
//        std::cout << args->at(i) << " ";
//    }
    std::cout << ")\n";
}

void FunToken::setArgCount(int cnt) {
    argCount = cnt;
}

int FunToken::getArgCount() {
    return argCount;
}

void FunToken::setArgs(std::vector<double> *args) {
    this->args = args;
}

double FunToken::SIN(double t, ...) {
    va_list args;
    va_start(args, t);
    double x = va_arg(args, double);
    va_end(args);
    return sin(x * PI / 180);
}

double FunToken::COS(double t, ...) {
    va_list args;
    va_start(args, t);
    double x = va_arg(args, double);
    va_end(args);
    return cos(x * PI / 180.0);
}

double FunToken::TAN(double t, ...) {
    va_list args;
    va_start(args, t);
    double x = va_arg(args, double);
    va_end(args);
    return tan(x * PI / 180);
}

double FunToken::LOG(double t, ...) {
    va_list args;
    va_start(args, t);
    double x = va_arg(args, double);
    va_end(args);
    return log(x);
}

double FunToken::EXP(double t, ...) {
    va_list args;
    va_start(args, t);
    double x = va_arg(args, double);
    va_end(args);
    return exp(x);
}

double FunToken::SQRT(double t, ...) {
    va_list args;
    va_start(args, t);
    double x = va_arg(args, double);
    va_end(args);
    return sqrt(x);
}

double FunToken::POW(double t, ...) {
    va_list args;
    va_start(args, t);
    double x = va_arg(args, double);
    double y = va_arg(args, double);
    va_end(args);
    return pow(x, y);
}

double FunToken::HYPOT(double t, ...) {
    va_list args;
    va_start(args, t);
    double x = va_arg(args, double);
    double y = va_arg(args, double);
    va_end(args);
    return hypot(x, y);
}

double FunToken::LERP(double t, ...) {
    va_list args;
    va_start(args, t);
    double x = va_arg(args, double);
    double y = va_arg(args, double);
    double z = va_arg(args, double);
    va_end(args);
    return x + z * (y - x);
}

double FunToken::polynomial(double t, ...) {
    va_list args;
    va_start(args, t);
    double ans = 0;
    double a[(int)t];
    for(int i = 0; i < (int)t; i++)
        a[i] = va_arg(args, double);
    for(int i = 0; i < (int)t - 1; i++)
        ans += a[i] * pow(a[(int)t - 1], i);
    va_end(args);
    return ans;
}

int FunToken::getNeededArgCount() {
    if (funcs->count(rawValue)){
        return funcs->find(rawValue)->second.second;
    } else {
        return specials->find(rawValue)->second.second;
    }
}

double FunToken::specialEval() {
    std::deque<Token *>* rpnQueue = (*specials)[rawValue].first;
    std::stack<double> evalStack;
    while (!rpnQueue->empty()) {
        Token *token = rpnQueue->front();
        rpnQueue->pop_front();
        NumToken *num = dynamic_cast<NumToken *>(token);
        OptToken *opt = dynamic_cast<OptToken *>(token);
        FunToken *fun = dynamic_cast<FunToken *>(token);
        PhdToken *phd = dynamic_cast<PhdToken *>(token);

        if (num) {
            evalStack.push(num->eval());
        } else if (phd) {
            evalStack.push(args->at(args->size() - phd->eval()));
        } else if (opt) {
            if (evalStack.size() < 2)
                throw std::domain_error("Invalid Equation");
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
    return evalStack.top();
}
