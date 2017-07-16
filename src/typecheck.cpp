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

struct TypeChecker : Visitor {
    // TODO: Scoping (both variables and functions).
    std::unordered_map<std::string, VarDecl*> context;
    std::vector<TypeError> errors;

    void AfterBlock(Block& block) override;
    void AfterId(Id& id) override;
    void AfterBinary(Binary& binary) override;
    void AfterIntLit(IntLit& int_lit) override;
    void AfterFloatLit(FloatLit& float_lit) override;
    void AfterParsedType(ParsedType& type) override;
    void AfterVarDecl(VarDecl& decl) override;
    void AfterLocalVarDecl(LocalVarDecl& decl) override;
    void AfterParam(Param& param) override;
    void AfterFnProto(FnProto& proto) override;
    void AfterFnDecl(FnDecl& fn) override;
};

std::vector<TypeError> TypeCheck(Node& ast) {
    TypeChecker v;
    ast.Accept(v);
    return v.errors;
}

void TypeChecker::AfterBlock(Block& block) {
    block.type = block.exprs.empty()
        ? make_shared<UnitType>()
        : block.exprs.rbegin()->get()->type;
}

void TypeChecker::AfterId(Id& id) {
    auto it = context.find(id.name.str_val);
    if (it == context.end()) {
        errors.emplace_back(BuildStr("Undeclared variable \'", id.name, '\''),
                            id.loc);
        return;
    }
    auto decl = it->second;
    id.type = decl->type;
    id.resolved = decl;
}

void TypeChecker::AfterBinary(Binary& binary) {
    auto l = binary.lhs->type;
    auto r = binary.rhs->type;
    if (!l || !r) return;
    if (!l->Equals(r.get())) {
        errors.emplace_back(
            BuildStr("Invalid operands to binary operator ",
                     '\'', binary.op, '\'',
                     " (", *l, " and ", *r, ')'),
            binary.op.loc);
        return;
    }
    binary.type = l;
}

void TypeChecker::AfterIntLit(IntLit& int_lit) {
    int_lit.type = make_shared<IntType>(64, /*signed*/ true);
}

void TypeChecker::AfterFloatLit(FloatLit& float_lit) {
    float_lit.type = make_shared<FloatType>(64);
}

void TypeChecker::AfterParsedType(ParsedType& type) {
    if (type.type) return; // Pre-filled, probably as Unit.

    auto str = type.name.str_val;
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
        type.type = make_shared<IntType>(bits, signd);
    } else {
        errors.emplace_back(BuildStr("Unrecognized type ", str), type.loc);
    }
}

void TypeChecker::AfterVarDecl(VarDecl& decl) {
    auto prev = context.find(decl.name.str_val);
    if (prev != context.end()) {
        errors.emplace_back(
            BuildStr("Variable \'", decl.name, "\' already declared at ",
                     prev->second->name.loc),
            decl.loc);
        return;
    }
    context[decl.name.str_val] = &decl;
}

void TypeChecker::AfterLocalVarDecl(LocalVarDecl& decl) {
    decl.type = decl.init->type;
    TypeChecker::AfterVarDecl(decl);
}

void TypeChecker::AfterParam(Param& param) {
    param.type = param.parsed_type->type;
    TypeChecker::AfterVarDecl(param);
}

// TODO: Recursion.
void TypeChecker::AfterFnProto(FnProto& proto) {
    std::vector<shared_ptr<Type>> arg_types(proto.args.size());
    for (size_t i = 0; i < arg_types.size(); ++i) {
        arg_types[i] = proto.args[i]->parsed_type->type;
        if (!arg_types[i]) return;
    }
    auto ret_type = proto.ret_type->type;
    if (!ret_type) return;
    proto.fn_type = make_shared<FnType>(arg_types, ret_type);
}

void TypeChecker::AfterFnDecl(FnDecl& fn) {
    auto body_type = fn.body->type.get();
    auto ret_type = fn.proto->fn_type->ret_type.get();
    if (!body_type || !ret_type) return;

    // We allow a type mismatch if the function returns Unit.
    // TODO: Replace typeid with a better check.
    if (!body_type->Equals(ret_type) && typeid(*ret_type) != typeid(UnitType)) {
        errors.push_back(Expected(*fn.proto->fn_type->ret_type,
                                  *fn.body->type,
                                  fn.body->loc));
        return;
    }
}
