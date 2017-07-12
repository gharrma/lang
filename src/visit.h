#pragma once
#include "ast.h"

class Visitor {
public:
    virtual ~Visitor() {}

    virtual void BeforeNode(Node& node) {}

    virtual void BeforeExpr(Expr& expr) { BeforeNode(expr); }
    virtual void BeforeId(Id& id) { BeforeExpr(id); }
    virtual void BeforeBinary(Binary& binary) { BeforeExpr(binary); }
    virtual void BeforeLit(Lit& lit) { BeforeExpr(lit); }
    virtual void BeforeIntLit(IntLit& int_lit) { BeforeLit(int_lit); }
    virtual void BeforeFloatLit(FloatLit& float_lit) { BeforeLit(float_lit); }

    virtual void BeforeParsedType(ParsedType& type) { BeforeNode(type); }
    virtual void BeforeVarDecl(VarDecl& decl) { BeforeNode(decl); }
    virtual void BeforeFnProto(FnProto& proto) { BeforeNode(proto); }
    virtual void BeforeFnDecl(FnDecl& decl) { BeforeNode(decl); }

    virtual void AfterNode(Node& node) {}

    virtual void AfterExpr(Expr& expr) { AfterNode(expr); }
    virtual void AfterId(Id& id) { AfterExpr(id); }
    virtual void AfterBinary(Binary& binary) { AfterExpr(binary); }
    virtual void AfterLit(Lit& lit) { AfterExpr(lit); }
    virtual void AfterIntLit(IntLit& int_lit) { AfterLit(int_lit); }
    virtual void AfterFloatLit(FloatLit& float_lit) { AfterLit(float_lit); }

    virtual void AfterParsedType(ParsedType& type) { AfterNode(type); }
    virtual void AfterVarDecl(VarDecl& decl) { AfterNode(decl); }
    virtual void AfterFnProto(FnProto& proto) { AfterNode(proto); }
    virtual void AfterFnDecl(FnDecl& decl) { AfterNode(decl); }
};
