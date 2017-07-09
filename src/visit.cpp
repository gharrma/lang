#include "visit.h"

#include "ast.h"

void Id::Accept(Visitor& v) { v.VisitId(this); }

void Binary::Accept(Visitor& v) {
    lhs->Accept(v);
    rhs->Accept(v);
    v.VisitBinary(this);
}

void IntLit::Accept(Visitor& v) { v.VisitIntLit(this); }

void FloatLit::Accept(Visitor& v) { v.VisitFloatLit(this); }
