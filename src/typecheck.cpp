#include "typecheck.h"

#include <unordered_map>
#include "ast.h"
#include "type.h"
#include "util.h"
#include "visit.h"

using std::make_shared;

struct TypeVisitor : Visitor {
    std::unordered_map<std::string, VarDecl*> context; // TODO: Scoping.
    std::vector<TypeError> errors;

    void AfterId(Id* id) override;
    void AfterBinary(Binary* binary) override;
    void AfterIntLit(IntLit* int_lit) override;
    void AfterFloatLit(FloatLit* float_lit) override;
    void AfterParsedType(ParsedType* type) override;
    void AfterVarDecl(VarDecl* decl) override;
    void AfterFnProto(FnProto* proto) override;
};

void TypeVisitor::AfterId(Id* id) {
    auto it = context.find(id->name.str_val);
    if (it == context.end()) {
        errors.emplace_back(BuildStr("Undeclared variable ", id->name),
                            id->loc);
        return;
    }
    id->type = it->second->parsed_type->type;
}

void TypeVisitor::AfterBinary(Binary* binary) {
    auto l = binary->lhs->type;
    auto r = binary->rhs->type;
    if (!l || !r) return;
    if (!l->Equals(r.get())) {
        errors.emplace_back(
            BuildStr("Invalid operands to binary operator ",
                     '\'', binary->op, '\'',
                     " (", l, " and ", r, ')'),
            binary->op.loc);
        return;
    }
    binary->type = l;
}

void TypeVisitor::AfterIntLit(IntLit* int_lit) {
    int_lit->type = make_shared<IntType>(64, /*signed*/ true);
}

void TypeVisitor::AfterFloatLit(FloatLit* float_lit) {
    float_lit->type =  make_shared<FloatType>(64);
}

void TypeVisitor::AfterParsedType(ParsedType* type) {
    if (type->type) return; // Pre-filled, probably as Unit.

    auto str = type->name.str_val;
    if (str == "i8")  type->type = make_shared<IntType>(8,  /*signed*/ true);
    if (str == "i16") type->type = make_shared<IntType>(16, /*signed*/ true);
    if (str == "i32") type->type = make_shared<IntType>(32, /*signed*/ true);
    if (str == "i64") type->type = make_shared<IntType>(64, /*signed*/ true);

    if (str == "u8")  type->type = make_shared<IntType>(8,  /*signed*/ false);
    if (str == "u16") type->type = make_shared<IntType>(16, /*signed*/ false);
    if (str == "u32") type->type = make_shared<IntType>(32, /*signed*/ false);
    if (str == "u64") type->type = make_shared<IntType>(64, /*signed*/ false);

    errors.emplace_back("Unrecognized type", type->loc);
}

void TypeVisitor::AfterVarDecl(VarDecl* decl) {
    auto prev = context.find(decl->name.str_val);
    if (prev != context.end()) {
        errors.emplace_back(
            BuildStr("Variable ", decl->name, " already declared at ",
                     prev->second->name.loc),
            decl->loc);
        return;
    }
    context[decl->name.str_val] = decl;
}

// TODO: Recursion.
void TypeVisitor::AfterFnProto(FnProto* proto) {
    // TODO: Implement function types.
}

std::vector<TypeError> TypeCheck(Node& ast) {
    TypeVisitor v;
    ast.Accept(v);
    return v.errors;
}
