#pragma once
#include "llvm/IR/Value.h"

struct Expr;

// TODO: Pass in context?
llvm::Value* EmitExpr(Expr* expr);
