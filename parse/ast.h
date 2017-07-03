#ifndef AST_H
#define AST_H
#include <memory>
#include <string>
#include "location.h"
#include "token.h"

using std::unique_ptr;
using std::move;

namespace parse {

struct Node {
    Location loc;
    Node(Location loc): loc(loc) {}
    virtual ~Node() {}
    virtual void PrettyPrint(std::ostream& os) const = 0;
};

struct Number {
    enum Kind { kNothing, kInt, kFloat };
    Kind kind;
    union { int64_t int_val; double float_val; };
    Number(): kind(kNothing) {}
    Number(int64_t val): kind(kInt), int_val(val) {}
    Number(double val): kind(kFloat), float_val(val) {}
    explicit operator bool() const { return kind != kNothing; }
};

std::ostream& operator<<(std::ostream& os, const Number& num);

struct Expr : Node {
    Expr(Location loc): Node(loc) {}
    virtual Number Eval() const { return Number(); };
};

struct Id : Expr {
    std::string name;
    Id(Location loc, std::string name): Expr(loc), name(name) {}
    void PrettyPrint(std::ostream& os) const { os << name; }
};

struct Binary : Expr {
    Token op;
    unique_ptr<Expr> lhs, rhs;
    Binary(Token op, unique_ptr<Expr> lhs, unique_ptr<Expr> rhs)
        : Expr(Location(lhs->loc, rhs->loc))
        , op(op)
        , lhs(move(lhs))
        , rhs(move(rhs)) {}
    void PrettyPrint(std::ostream& os) const;
    Number Eval() const;
};

struct Prefix : Expr {
    Token op;
    unique_ptr<Expr> expr;
    Prefix(Token op, unique_ptr<Expr> expr)
        : Expr(Location(op.loc, expr->loc)), op(op), expr(move(expr)) {}
    void PrettyPrint(std::ostream& os) const;
};

template <typename T>
struct Lit : Expr {
    T val;
    Lit(Location loc, T val): Expr(loc), val(val) {}
    void PrettyPrint(std::ostream& os) const { os << val; };
    Number Eval() const { return Number(val); }
};

template <> // Custom pretty printing for float literals.
void Lit<decltype(Token::float_val)>::PrettyPrint(std::ostream& os) const;

template <> // Custom constant evalutation for string literals.
Number Lit<decltype(Token::str_val)>::Eval() const;

} // parse

#endif // AST_H
