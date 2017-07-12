#include "visit.h"

void Id::Accept(Visitor& v) {
    v.BeforeId(*this);
    v.AfterId(*this);
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

void VarDecl::Accept(Visitor& v) {
    v.BeforeVarDecl(*this);
    parsed_type->Accept(v);
    v.AfterVarDecl(*this);
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
