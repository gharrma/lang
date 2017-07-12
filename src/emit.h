#pragma once

struct Node;
struct Expr;

namespace llvm {
    class Module;
    class Value;
}

void Emit(Node& ast, llvm::Module& mod);

llvm::Value* EmitExpr(Expr& expr, llvm::Module& mod);
