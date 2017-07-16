#pragma once
#include <vector>
#include <unordered_map>
#include "ast.h"
#include "error.h"
#include "visit.h"

class TypeChecker : public Visitor {
public:
    std::vector<TypeError> errors;

    void AfterBlock(Block& block) override;
    void AfterId(Id& id) override;
    void AfterCall(Call& call) override;
    void AfterBinary(Binary& binary) override;
    void AfterIntLit(IntLit& int_lit) override;
    void AfterFloatLit(FloatLit& float_lit) override;
    void AfterParsedType(ParsedType& type) override;
    void AfterVarDecl(VarDecl& decl) override;
    void AfterLocalVarDecl(LocalVarDecl& decl) override;
    void AfterParam(Param& param) override;
    void AfterFnProto(FnProto& proto) override;
    void BeforeFnDecl(FnDecl& fn) override;
    void AfterFnDecl(FnDecl& fn) override;

private:
    // TODO: Scoping (both variables and functions).
    std::unordered_map<std::string, VarDecl*> vars_;
    std::unordered_map<std::string, FnDecl*> fns_;
};
