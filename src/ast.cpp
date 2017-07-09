#include "ast.h"

#include <iostream>
#include "error.h"
#include "util.h"
#include "visit.h"

std::ostream& operator<<(std::ostream& os, const Node& node) {
    node.Print(os);
    return os;
}

void Binary::Print(std::ostream& os) const {
    os << '(';
    lhs->Print(os);
    os << ' ' << op << ' ';
    rhs->Print(os);
    os << ')';
}

void FloatLit::Print(std::ostream& os) const {
    os << val;
    if (val == static_cast<int64_t>(val)) {
        os << ".0";
    }
}
