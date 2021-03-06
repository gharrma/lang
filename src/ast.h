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
class PrettyPrinter;

struct Node {
    Loc loc; // TODO: Considering converting to method.
    explicit Node(Loc loc): loc(loc) {}
    virtual ~Node() {}
    virtual void Print(PrettyPrinter& pp) const = 0;
    virtual void Accept(Visitor& v) = 0; // See visit.cpp.
};

std::ostream& operator<<(std::ostream& os, const Node& node);

struct ParsedType : Node {
    Token name;
    std::shared_ptr<Type> type;
    explicit ParsedType(Token name): Node(name.loc), name(name) {}
    ParsedType(Loc loc, std::shared_ptr<Type> type): Node(loc), type(type) {}
    void Print(PrettyPrinter& pp) const override;
    void Accept(Visitor& v) override;
};

struct Expr : Node {
    std::shared_ptr<Type> type; // Set by type check.
    explicit Expr(Loc loc): Node(loc) {}
};

struct VarDecl : Expr {
    Token name;
    VarDecl(Loc loc, Token name): Expr(loc), name(name) {}
};

struct LocalVarDecl : VarDecl {
    std::unique_ptr<Expr> init;
    LocalVarDecl(Token name, std::unique_ptr<Expr> init)
        : VarDecl(Loc(name.loc, init->loc), name)
        , init(std::move(init)) {}
    void Print(PrettyPrinter& pp) const override;
    void Accept(Visitor& v) override;
};

struct Param : VarDecl {
    std::unique_ptr<ParsedType> parsed_type;
    Param(Token name, std::unique_ptr<ParsedType> parsed_type)
        : VarDecl(Loc(name.loc, parsed_type->loc), name)
        , parsed_type(std::move(parsed_type)) {}
    void Print(PrettyPrinter& pp) const override;
    void Accept(Visitor& v) override;
};

struct Block : Expr {
    std::vector<std::unique_ptr<Expr>> exprs;
    Block(Loc loc, std::vector<std::unique_ptr<Expr>> exprs)
        : Expr(loc)
        , exprs(std::move(exprs)) {}
    void Print(PrettyPrinter& pp) const override;
    void Accept(Visitor& v) override;
};

struct Id : Expr {
    Token name;
    VarDecl* resolved = nullptr; // Set by type check.
    explicit Id(Token name): Expr(name.loc), name(name) {}
    void Print(PrettyPrinter& pp) const override;
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
    void Print(PrettyPrinter& pp) const override;
    void Accept(Visitor& v) override;
};

struct Lit : Expr {
    Lit(Loc loc): Expr(loc) {}
};

struct IntLit : Lit {
    IntLitRep val;
    IntLit(Loc loc, IntLitRep val): Lit(loc), val(val) {}
    void Print(PrettyPrinter& pp) const override;
    void Accept(Visitor& v) override;
};

struct FloatLit : Lit {
    FloatLitRep val;
    FloatLit(Loc loc, FloatLitRep val): Lit(loc), val(val) {}
    void Print(PrettyPrinter& pp) const override;
    void Accept(Visitor& v) override;
};

// TODO: Rename this, as it's not quite a function prototype.
//       (Closer to parsed function type).
struct FnProto : Node {
    std::vector<std::unique_ptr<Param>> args;
    std::unique_ptr<ParsedType> ret_type;
    std::shared_ptr<FnType> fn_type;
    FnProto(Loc start_loc,
            std::vector<std::unique_ptr<Param>> args,
            std::unique_ptr<ParsedType> ret_type)
        : Node(Loc(start_loc, ret_type->loc))
        , args(std::move(args))
        , ret_type(std::move(ret_type)) {}
    void Print(PrettyPrinter& pp) const override;
    void Accept(Visitor& v) override;
};

// Fold FnProto into FnDecl, and make FnDecl an expr.
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
    void Print(PrettyPrinter& pp) const override;
    void Accept(Visitor& v) override;
};

struct Call : Expr {
    Token name;
    std::vector<std::unique_ptr<Expr>> args;
    FnDecl* resolved = nullptr; // Set by type check.
    Call(Loc loc, Token name, std::vector<std::unique_ptr<Expr>> args)
        : Expr(loc)
        , name(name)
        , args(std::move(args)) {}
    void Print(PrettyPrinter& pp) const override;
    void Accept(Visitor& v) override;
};
