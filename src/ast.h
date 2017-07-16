#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "location.h"
#include "token.h"
#include "type.h"

// TODO: May be desirable to decouple types.
// TODO: Might want to assign each node an index (to optimize visitors).

class Visitor;

struct Node {
    Loc loc; // TODO: Considering converting to method.
    Node(Loc loc): loc(loc) {}
    virtual ~Node() {}
    virtual void Print(std::ostream& os) const = 0;
    virtual void Accept(Visitor& v) = 0; // See visit.cpp.
};

std::ostream& operator<<(std::ostream& os, const Node& node);

struct ParsedType : Node {
    Token name;
    std::shared_ptr<Type> type;
    ParsedType(Token name): Node(name.loc), name(name) {}
    ParsedType(Loc loc, std::shared_ptr<Type> type): Node(loc), type(type) {}
    void Print(std::ostream& os) const override;
    void Accept(Visitor& v) override;
};

struct VarDecl : Node {
    Token name;
    std::unique_ptr<ParsedType> parsed_type; // TODO: No need for unique_ptr?
    VarDecl(Token name, std::unique_ptr<ParsedType> parsed_type)
        : Node(Loc(name.loc, parsed_type->loc))
        , name(name)
        , parsed_type(std::move(parsed_type)) {}
    void Print(std::ostream& os) const override;
    void Accept(Visitor& v) override;
};

struct Expr : Node {
    std::shared_ptr<Type> type;
    Expr(Loc loc): Node(loc) {}
};

struct Block : Expr {
    std::vector<std::unique_ptr<Expr>> exprs;
    Block(Loc loc, std::vector<std::unique_ptr<Expr>> exprs)
        : Expr(loc)
        , exprs(std::move(exprs)) {}
    void Print(std::ostream& os) const override;
    void Accept(Visitor& v) override;
};

// TODO: Add pointer to resolved variable?
struct Id : Expr {
    Token name;
    VarDecl* resolved = nullptr; // Set by type check.
    Id(Token name): Expr(name.loc), name(name) {}
    void Print(std::ostream& os) const override { os << name; }
    void Accept(Visitor& v) override;
};

struct Binary : Expr {
    Token op;
    std::unique_ptr<Expr> lhs, rhs;
    Binary(Token op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
        : Expr(Loc(lhs->loc, rhs->loc))
        , op(op)
        , lhs(std::move(lhs))
        , rhs(std::move(rhs)) {}
    void Print(std::ostream& os) const override;
    void Accept(Visitor& v) override;
};

struct Lit : Expr {
    Lit(Loc loc): Expr(loc) {}
};

struct IntLit : Lit {
    IntLitRep val;
    IntLit(Loc loc, IntLitRep val): Lit(loc), val(val) {}
    void Print(std::ostream& os) const override { os << val; };
    void Accept(Visitor& v) override;
};

struct FloatLit : Lit {
    FloatLitRep val;
    FloatLit(Loc loc, FloatLitRep val): Lit(loc), val(val) {}
    void Print(std::ostream& os) const override { os << val; };
    void Accept(Visitor& v) override;
};

// TODO: Rename this, as it's not quite a function prototype.
//       (Closer to parsed function type).
struct FnProto : Node {
    std::vector<std::unique_ptr<VarDecl>> args;
    std::unique_ptr<ParsedType> ret_type;
    std::shared_ptr<FnType> fn_type;
    FnProto(Loc start_loc,
            std::vector<std::unique_ptr<VarDecl>> args,
            std::unique_ptr<ParsedType> ret_type)
        : Node(Loc(start_loc, ret_type->loc))
        , args(std::move(args))
        , ret_type(std::move(ret_type)) {}
    void Print(std::ostream& os) const override;
    void Accept(Visitor& v) override;
};

struct FnDecl : Node {
    Token name;
    std::unique_ptr<FnProto> proto;
    std::unique_ptr<Expr> body;
    FnDecl(Loc start_loc, Token name,
           std::unique_ptr<FnProto> proto,
           std::unique_ptr<Expr> body)
        : Node(Loc(start_loc, body->loc))
        , name(name)
        , proto(std::move(proto))
        , body(std::move(body)) {}
    void Print(std::ostream& os) const override;
    void Accept(Visitor& v) override;
};
