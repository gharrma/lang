#include "ast.h"

#include <iostream>

namespace parse {

void Binary::PrettyPrint(std::ostream& os) const {
    os << '(';
    lhs->PrettyPrint(os);
    os << ' ' << op << ' ';
    rhs->PrettyPrint(os);
    os << ')';
}

void Prefix::PrettyPrint(std::ostream& os) const {
    os << op;
    expr->PrettyPrint(os);
};

} // namespace parse
