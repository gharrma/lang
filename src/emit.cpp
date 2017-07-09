#include "emit.h"

#include "ast.h"
#include "type.h"
#include "util.h"

using namespace llvm;

Value* Binary::Emit(EmitContext& v) const {
    auto l = lhs->Emit(v);
    auto r = rhs->Emit(v);
    if (auto cast = dynamic_cast<IntType*>(type.get())) {
        switch (op.kind) {
            case kPlus:  return v.builder.CreateAdd(l, r, "plus");
            case kMinus: return v.builder.CreateSub(l, r, "minus");
            case kTimes: return v.builder.CreateMul(l, r, "times");
            case kDiv: case kMod: {
                if (op.kind == kDiv) {
                    if (cast->signd)
                         return v.builder.CreateSDiv(l, r, "div");
                    else return v.builder.CreateUDiv(l, r, "div");
                } else {
                    if (cast->signd)
                         return v.builder.CreateSRem(l, r, "mod");
                    else return v.builder.CreateURem(l, r, "mod");
                }
            }
            default:
                LOG(FATAL) << "Unhandled binop " << op;
        }
    }
    if (dynamic_cast<FloatType*>(type.get())) {
        switch (op.kind) {
            case kPlus:  return v.builder.CreateFAdd(l, r, "plus");
            case kMinus: return v.builder.CreateFSub(l, r, "minus");
            case kTimes: return v.builder.CreateFMul(l, r, "times");
            case kDiv:   return v.builder.CreateFDiv(l, r, "div");
            default:
                LOG(FATAL) << "Unhandled binop " << op;
        }
    }
    LOG(FATAL) << "Unhandled binop operand types";
    return nullptr;
}

Value* IntLit::Emit(EmitContext& v) const {
    static constexpr unsigned bits =
        CHAR_BIT * sizeof(decltype(Token::int_val));
    return ConstantInt::get(v.context, APInt(bits, val));
}

Value* FloatLit::Emit(EmitContext& v) const {
    return ConstantFP::get(v.context, APFloat(val));
}
