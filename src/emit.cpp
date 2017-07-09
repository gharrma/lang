#include "emit.h"

#include <unordered_map>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "ast.h"
#include "type.h"
#include "util.h"
#include "visit.h"

using namespace llvm;

struct Emitter : Visitor {
    LLVMContext context;
    std::unique_ptr<Module> mod;
    IRBuilder<> builder;
    std::unordered_map<Expr*, Value*> vals;
    Emitter()
        : mod(make_unique<Module>("default module", context))
        , builder(context) {}

    void AfterId(Id* id) override;
    void AfterBinary(Binary* binary) override;
    void AfterIntLit(IntLit* int_lit) override;
    void AfterFloatLit(FloatLit* float_lit) override;
    void AfterVarDecl(VarDecl* decl) override;
    void AfterFnDecl(FnDecl* fn) override;
};

Value* EmitExpr(Expr* expr) {
    Emitter emitter;
    expr->Accept(emitter);
    return emitter.vals[expr];
}

void Emitter::AfterId(Id* id) {
    // TODO
}

void Emitter::AfterBinary(Binary* binary) {
    auto l = vals.at(binary->lhs.get());
    auto r = vals.at(binary->rhs.get());
    auto op = binary->op;
    auto op_kind = op.kind;
    Value* val = nullptr;
    if (auto int_type = dynamic_cast<IntType*>(binary->type.get())) {
             if (op_kind == kPlus)  val = builder.CreateAdd(l, r, "plus");
        else if (op_kind == kMinus) val = builder.CreateSub(l, r, "minus");
        else if (op_kind == kTimes) val = builder.CreateMul(l, r, "times");
        else if (op_kind == kDiv || op_kind == kMod) {
            if (op_kind == kDiv) {
                int_type->signd
                    ? val = builder.CreateSDiv(l, r, "div")
                    : val = builder.CreateUDiv(l, r, "div");
            } else {
                int_type->signd
                    ? val = builder.CreateSRem(l, r, "mod")
                    : val = builder.CreateURem(l, r, "mod");
            }
        }
    }
    else if (dynamic_cast<FloatType*>(binary->type.get())) {
             if (op_kind == kPlus)  val = builder.CreateFAdd(l, r, "plus");
        else if (op_kind == kMinus) val = builder.CreateFSub(l, r, "minus");
        else if (op_kind == kTimes) val = builder.CreateFMul(l, r, "times");
        else if (op_kind == kDiv)   val = builder.CreateFDiv(l, r, "div");
    }
    if (!val) LOG(FATAL) << "Binary operator \'" << op << "\' not translated";
    vals[binary] = val;
}

void Emitter::AfterIntLit(IntLit* int_lit) {
    static constexpr unsigned bits =
        CHAR_BIT * sizeof(decltype(Token::int_val));
    vals[int_lit] = ConstantInt::get(context, APInt(bits, int_lit->val));
}

void Emitter::AfterFloatLit(FloatLit* float_lit) {
    vals[float_lit] = ConstantFP::get(context, APFloat(float_lit->val));
}

void Emitter::AfterVarDecl(VarDecl* decl) {
    // TODO
}

void Emitter::AfterFnDecl(FnDecl* fn) {
    // TODO
}
