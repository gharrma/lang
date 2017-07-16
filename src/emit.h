#pragma once
#include <unordered_map>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "ast.h"
#include "visit.h"

namespace llvm {
    class LLVMContext;
    class Value;
}

struct Emitter : Visitor {
    llvm::Module& mod;
    llvm::LLVMContext& context;
    llvm::IRBuilder<> builder;
    std::unordered_map<Expr*, llvm::Value*> vals;
    std::unordered_map<FnDecl*, llvm::Function*> fns;

    Emitter(llvm::Module& mod)
        : mod(mod)
        , context(mod.getContext())
        , builder(context) {}

    llvm::Type* GetLlvmType(const ::Type* type);

    void AfterBlock(Block& block) override;
    void AfterId(Id& id) override;
    void AfterCall(Call& call) override;
    void AfterBinary(Binary& binary) override;
    void AfterIntLit(IntLit& int_lit) override;
    void AfterFloatLit(FloatLit& float_lit) override;
    void AfterLocalVarDecl(LocalVarDecl& decl) override;
    void BeforeFnDecl(FnDecl& fn) override;
    void AfterFnDecl(FnDecl& fn) override;
};
