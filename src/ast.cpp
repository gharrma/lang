#include "ast.h"

#include <iostream>
#include <sstream>
#include "error.h"
#include "util.h"
#include "visit.h"

std::ostream& operator<<(std::ostream& os, const Node& node) {
    node.Print(os);
    return os;
}

void Binary::Print(std::ostream& os) const {
    os << '(' << *lhs << ' ' << op << ' ' << *rhs << ')';
}

void FloatLit::Print(std::ostream& os) const {
    os << val;
    if (val == static_cast<int64_t>(val)) {
        os << ".0";
    }
}

void ParsedType::Print(std::ostream& os) const {
    // Unnamed parsed type probably means implicit Unit.
    if (name) os << name;
}

void VarDecl::Print(std::ostream& os) const {
    os << name << ' ' << *parsed_type;
}

void FnProto::Print(std::ostream& os) const {
    if (!args.empty()) {
        os << '(';
        for (auto it = args.begin(), e = args.end(); it != e; ++it) {
            if (it != args.begin())
                os << ", ";
            os << **it;
        }
        os << ')';
    }
    std::ostringstream ss;
    ss << *ret_type;
    if (!ss.str().empty()) {
        os << ' ' << ss.str();
    }
}

void FnDecl::Print(std::ostream& os) const {
    os << kFn << ' ' << name << *proto << " = " << *body;
}
