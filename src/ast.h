#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "llvm/IR/Value.h"
#include "location.h"
#include "token.h"
#include "type.h"

// TODO: Turn Emit into visitor, and keep AST decoupled from LLVM back end.
//       Also possible to decouple types.
//       In both cases would want to assign each AST node an index.

struct EmitContext;
class Visitor;

struct Node {
    Loc loc;
    Node(Loc loc): loc(loc) {}
    virtual ~Node() {}
    virtual void Print(std::ostream& os) const = 0;
    virtual void Accept(Visitor& v) = 0; // See visit.cpp.
};

std::ostream& operator<<(std::ostream& os, const Node& node);

struct Expr : Node {
    std::shared_ptr<Type> type;
    Expr(Loc loc): Node(loc) {}
    virtual llvm::Value* Emit(EmitContext& v) const = 0; // See emit.cpp.
};

struct Id : Expr {
    Token name;
    Id(Token name): Expr(name.loc), name(name) {}
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

struct FnProto : Node {
    std::vector<std::unique_ptr<VarDecl>> args;
    std::unique_ptr<ParsedType> ret_type;
    std::shared_ptr<Type> fn_type;
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
