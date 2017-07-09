#pragma once
#include <memory>
#include <string>
#include <iostream>
#include "llvm/IR/Value.h"
#include "location.h"
#include "token.h"
#include "type.h"

struct EmitContext; // TODO: Consider turning into visitor.
class Visitor;

struct Node {
    Loc loc;
    std::shared_ptr<Type> type;
    Node(Loc loc): loc(loc) {}
    virtual ~Node() {}
    virtual void Print(std::ostream& os) const = 0;
    virtual void Accept(Visitor& v) = 0; // See visit.cpp.
};

std::ostream& operator<<(std::ostream& os, const Node& node);

struct Expr : Node {
    Expr(Loc loc): Node(loc) {}
    virtual llvm::Value* Emit(EmitContext& v) const = 0; // See emit.cpp.
};

struct Id : Expr {
    std::string name;
    Id(Loc loc, std::string name): Expr(loc), name(name) {}
    void Print(std::ostream& os) const override { os << name; }
    void Accept(Visitor& v) override;
    llvm::Value* Emit(EmitContext& v) const override { return nullptr; }
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
    llvm::Value* Emit(EmitContext& v) const override;
};

struct Lit : Expr {
    Lit(Loc loc): Expr(loc) {}
};

struct IntLit : Lit {
    decltype(Token::int_val) val;
    IntLit(Loc loc, decltype(val) val): Lit(loc), val(val) {}
    void Print(std::ostream& os) const override { os << val; };
    void Accept(Visitor& v) override;
    llvm::Value* Emit(EmitContext& v) const override;
};

struct FloatLit : Lit {
    decltype(Token::float_val) val;
    FloatLit(Loc loc, decltype(val) val): Lit(loc), val(val) {}
    void Print(std::ostream& os) const override;
    void Accept(Visitor& v) override;
    llvm::Value* Emit(EmitContext& v) const override;
};
