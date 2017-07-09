#pragma once
#include "ast.h"

class Visitor {
public:
    virtual ~Visitor() {}
    virtual void VisitNode(Node* node) {}
    virtual void VisitExpr(Expr* expr) { VisitNode(expr); }
    virtual void VisitId(Id* id) { VisitExpr(id); }
    virtual void VisitBinary(Binary* binary) { VisitExpr(binary); }
    virtual void VisitLit(Lit* lit) { VisitExpr(lit); }
    virtual void VisitIntLit(IntLit* int_lit) { VisitLit(int_lit); }
    virtual void VisitFloatLit(FloatLit* float_lit) { VisitLit(float_lit); }
};
