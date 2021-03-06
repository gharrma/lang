#include "visit.h"

void Block::Accept(Visitor& v) {
    v.BeforeBlock(*this);
    for (auto& expr : exprs)
        expr->Accept(v);
    v.AfterBlock(*this);
}

void Id::Accept(Visitor& v) {
    v.BeforeId(*this);
    v.AfterId(*this);
}

void Call::Accept(Visitor& v) {
    v.BeforeCall(*this);
    for (auto& arg : args)
        arg->Accept(v);
    v.AfterCall(*this);
}

void Binary::Accept(Visitor& v) {
    v.BeforeBinary(*this);
    lhs->Accept(v);
    rhs->Accept(v);
    v.AfterBinary(*this);
}

void IntLit::Accept(Visitor& v) {
    v.BeforeIntLit(*this);
    v.AfterIntLit(*this);
}

void FloatLit::Accept(Visitor& v) {
    v.BeforeFloatLit(*this);
    v.AfterFloatLit(*this);
}

void ParsedType::Accept(Visitor& v) {
    v.BeforeParsedType(*this);
    v.AfterParsedType(*this);
}

void LocalVarDecl::Accept(Visitor& v) {
    v.BeforeLocalVarDecl(*this);
    init->Accept(v);
    v.AfterLocalVarDecl(*this);
}

void Param::Accept(Visitor& v) {
    v.BeforeParam(*this);
    parsed_type->Accept(v);
    v.AfterParam(*this);
}

void FnProto::Accept(Visitor& v) {
    v.BeforeFnProto(*this);
    for (auto& arg : args)
        arg->Accept(v);
    ret_type->Accept(v);
    v.AfterFnProto(*this);
}

void FnDecl::Accept(Visitor& v) {
    v.BeforeFnDecl(*this);
    proto->Accept(v);
    body->Accept(v);
    v.AfterFnDecl(*this);
}
