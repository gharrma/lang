#include "typecheck.h"

#include <unordered_map>
#include "ast.h"
#include "type.h"
#include "util.h"
#include "visit.h"

using std::shared_ptr;
using std::make_shared;

static TypeError Expected(const Type& expected, const Type& actual, Loc loc) {
    return TypeError(
        BuildStr("Expected type ", expected, ", instead found ", actual),
        loc);
}

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
    void AfterFnDecl(FnDecl* fn) override;
};

std::vector<TypeError> TypeCheck(Node& ast) {
    TypeVisitor v;
    ast.Accept(v);
    return v.errors;
}

void TypeVisitor::AfterId(Id* id) {
    auto it = context.find(id->name.str_val);
    if (it == context.end()) {
        errors.emplace_back(BuildStr("Undeclared variable \'", id->name, '\''),
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
    float_lit->type = make_shared<FloatType>(64);
}

void TypeVisitor::AfterParsedType(ParsedType* type) {
    if (type->type) return; // Pre-filled, probably as Unit.

    auto str = type->name.str_val;
    uint8_t bits = 0;
    bool signd = true;

    if (str == "i8")  bits = 8u;
    if (str == "i16") bits = 16u;
    if (str == "i32") bits = 32u;
    if (str == "i64") bits = 64u;

    if (str == "u8")  bits = 8u,  signd = false;
    if (str == "u16") bits = 16u, signd = false;
    if (str == "u32") bits = 32u, signd = false;
    if (str == "u64") bits = 64u, signd = false;

    if (bits) {
        type->type = make_shared<IntType>(bits, signd);
    } else {
        errors.emplace_back("Unrecognized type", type->loc);
    }
}

void TypeVisitor::AfterVarDecl(VarDecl* decl) {
    auto prev = context.find(decl->name.str_val);
    if (prev != context.end()) {
        errors.emplace_back(
            BuildStr("Variable \'", decl->name, "\' already declared at ",
                     prev->second->name.loc),
            decl->loc);
        return;
    }
    context[decl->name.str_val] = decl;
}

// TODO: Recursion.
void TypeVisitor::AfterFnProto(FnProto* proto) {
    std::vector<shared_ptr<Type>> arg_types(proto->args.size());
    for (size_t i = 0; i < arg_types.size(); ++i)
        arg_types[i] = proto->args[i]->parsed_type->type;
    auto ret_type = proto->ret_type->type;
    proto->fn_type = make_shared<FnType>(arg_types, ret_type);
}

void TypeVisitor::AfterFnDecl(FnDecl* fn) {
    if (!fn->body->type->Equals(fn->proto->fn_type->ret_type.get())) {
        errors.push_back(Expected(*fn->proto->fn_type->ret_type,
                                  *fn->body->type,
                                  fn->body->loc));
        return;
    }
}
