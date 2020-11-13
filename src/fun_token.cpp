//
// Created by S.Morteza Ezzabady on 11/8/20.
//

#include "fun_token.h"


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
    double ans = apply([this](auto...xs){return (*(*(funcs))[rawValue])(std::forward<decltype(xs)>(xs)...);}, a);
    return ans;
}

FunToken::FunToken(std::string str,
                   std::map<std::string, double(*)(double, ...)> *funcs) : Token(str), funcs{funcs}, argCount{0} {}

void FunToken::print() {
    std::cout << "function " << rawValue << "( ";
    for(int i = args->size() - 1; i >= 0; i--){
        std::cout << args->at(i) << " ";
    }
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
    double pi = 3.14159265359;
    va_list args;
    va_start(args, t);
    double x = va_arg(args, double);
    va_end(args);
    return sin(x * (pi / 180));
}

double FunToken::COS(double t, ...) {
    double pi = 3.14159265359;
    va_list args;
    va_start(args, t);
    double x = va_arg(args, double);
    va_end(args);
    return cos(x * (pi / 180));
}

double FunToken::TAN(double t, ...) {
    double pi = 3.14159265359;
    va_list args;
    va_start(args, t);
    double x = va_arg(args, double);
    va_end(args);
    return tan(x * (pi / 180));
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
