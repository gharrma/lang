#ifndef AST_H
#define AST_H
#include <memory>
#include <string>
#include "parse/location.h"

using std::unique_ptr;
using std::move;

namespace parse {

struct Node {
    Location loc;
    Node(Location loc): loc(loc) {}
    virtual ~Node() {}
};

struct Expr : Node {
    Expr(Location loc): Node(loc) {}
};

struct Binary : Expr {
    unique_ptr<Expr> lhs, rhs;
    Binary(unique_ptr<Expr> lhs, unique_ptr<Expr> rhs)
        : Expr(Location(lhs->loc, rhs->loc))
        , lhs(move(lhs))
        , rhs(move(rhs)) {}
};

struct Unary : Expr {
    unique_ptr<Expr> expr;
    Unary(unique_ptr<Expr> expr)
        : Expr(expr->loc), expr(move(expr)) {}
};

struct Lit : Expr {
    Lit(Location loc): Expr(loc) {}
};

struct IntLit : Expr {
    uint64_t val;
    IntLit(Location loc, uint64_t val): Expr(loc), val(val) {}
};

struct FloatLit : Expr {
    double val;
    FloatLit(Location loc, double val): Expr(loc), val(val) {}
};

struct StrLit : Expr {
    std::string val;
    StrLit(Location loc, std::string val): Expr(loc), val(val) {}
};

} // parse

#endif // AST_H
