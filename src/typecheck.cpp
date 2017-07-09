#include "typecheck.h"

#include "ast.h"
#include "type.h"
#include "util.h"
#include "visit.h"

struct TypeCheckVisitor : Visitor {
    std::vector<TypeError> errors;
    void VisitId(Id* id) override;
    void VisitBinary(Binary* binary) override;
    void VisitIntLit(IntLit* int_lit) override;
    void VisitFloatLit(FloatLit* float_lit) override;
};

void TypeCheckVisitor::VisitId(Id* id) {
    errors.emplace_back("Type check for IDs not implemented.", id->loc);
}

void TypeCheckVisitor::VisitBinary(Binary* binary) {
    auto l = binary->lhs->type;
    auto r = binary->rhs->type;
    if (!l || !r) return;
    if (!l->Equals(r.get())) {
        errors.emplace_back(
            BuildStr("Invalid operands to binary operator ",
                     '\'', binary->op, '\'',
                     " (", l, " and ", r, ')'),
            binary->op.loc
        );
    }
    binary->type = l;
}

void TypeCheckVisitor::VisitIntLit(IntLit* int_lit) {
    int_lit->type = std::make_shared<IntType>(64, /*signed*/ true);
}

void TypeCheckVisitor::VisitFloatLit(FloatLit* float_lit) {
    float_lit->type =  std::make_shared<FloatType>(64);
}

std::vector<TypeError> TypeCheck(Node& ast) {
    TypeCheckVisitor v;
    ast.Accept(v);
    return v.errors;
}
