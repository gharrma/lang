#include "codegen.h"
#include "ast.h"
// #include "llvm/ADT/APFloat.h"
// #include "llvm/ADT/APInt.h"

using namespace llvm;

Value* Binary::Codegen(CodegenContext& v) const {
    auto l = lhs->Codegen(v);
    auto r = rhs->Codegen(v);
    switch (op.kind) {
        case kPlus:  return v.builder.CreateAdd(l, r, "plus");
        case kMinus: return v.builder.CreateSub(l, r, "minus");
        case kTimes: return v.builder.CreateMul(l, r, "times");
        // case kDiv:   return v.builder.CreateDiv(l, r, "div");
        // case kMod:   return v.builder.CreateMod(l, r, "mod");
        default:
            return nullptr;
    }
}

template <>
Value* Lit<decltype(Token::int_val)>::Codegen(CodegenContext& v) const {
    return ConstantInt::get(v.context, APInt(64, val));
}

template <>
Value* Lit<decltype(Token::float_val)>::Codegen(CodegenContext& v) const {
    // return ConstantFP::get(v.context, APFloat(val));
    return nullptr;
}

template <>
Value* Lit<decltype(Token::str_val)>::Codegen(CodegenContext& v) const {
    return nullptr;
}
