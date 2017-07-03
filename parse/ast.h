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

struct Expr : Node {
    Expr(Location loc): Node(loc) {}
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
};

struct Prefix : Expr {
    Token op;
    unique_ptr<Expr> expr;
    Prefix(Token op, unique_ptr<Expr> expr)
        : Expr(op.loc), op(op), expr(move(expr)) {}
    void PrettyPrint(std::ostream& os) const;
};

template <typename T>
struct Lit : Expr {
    T val;
    Lit(Location loc, T val): Expr(loc), val(val) {}
    void PrettyPrint(std::ostream& os) const { os << val; };
};

} // parse

#endif // AST_H
