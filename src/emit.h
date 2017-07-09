#pragma once
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

struct EmitContext {
    llvm::LLVMContext context;
    std::unique_ptr<llvm::Module> mod;
    llvm::IRBuilder<> builder;
    EmitContext()
        : mod(llvm::make_unique<llvm::Module>("default module", context))
        , builder(context) {}
};
