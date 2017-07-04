#include "ast.h"

#include <iostream>
#include "base/logging.h"
#include "base/util.h"
#include "typecheck/types.h"

using typecheck::TypeError;

namespace parse {

std::ostream& operator<<(std::ostream& os, const Number& num) {
    switch (num.kind) {
        case Number::kNothing: return os << "unknown";
        case Number::kInt:     return os << num.int_val;
        case Number::kFloat:
            os << num.float_val;
            if (num.float_val == static_cast<int64_t>(num.float_val))
                os << ".0";
            return os;
    }
}

void Binary::PrettyPrint(std::ostream& os) const {
    os << '(';
    lhs->PrettyPrint(os);
    os << ' ' << op << ' ';
    rhs->PrettyPrint(os);
    os << ')';
}

#define RET_BINOP(type, op, l, r) \
    do if (type == Number::kInt) return Number(l.int_val op r.int_val); \
    else return Number(l.float_val op r.float_val); \
    while (0)

Number Binary::Eval() const {
    auto l = lhs->Eval();
    auto r = rhs->Eval();
    if (!l || !r) return Number();
    if (l.kind != r.kind) {
        throw TypeError(
            BuildStr("Invalid operands to binary operator \'", op, "\'."),
            op.loc);
    }
    auto type = l.kind;
    switch (op.kind) {
        case kPlus:  RET_BINOP(type, +, l, r);
        case kMinus: RET_BINOP(type, -, l, r);
        case kTimes: RET_BINOP(type, *, l, r);
        case kDiv:   RET_BINOP(type, /, l, r);
        case kMod:
            if (type == Number::kFloat)
                throw TypeError("Binary operator requires integer operands.",
                                op.loc);
            return Number(l.int_val % r.int_val);
        default:
            return Number();
    }
}

void Prefix::PrettyPrint(std::ostream& os) const {
    os << op;
    expr->PrettyPrint(os);
};

template <>
void Lit<double>::PrettyPrint(std::ostream& os) const {
    os << val;
    if (val == static_cast<int64_t>(val)) {
        os << ".0";
    }
}

template <>
Number Lit<decltype(Token::str_val)>::Eval() const { return Number(); }

} // namespace parse
