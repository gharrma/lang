#include "type.h"

#include <sstream>
#include <vector>
#include "llvm/ADT/STLExtras.h"
#include "ast.h"
#include "error.h"
#include "util.h"
#include "visit.h"

using llvm::make_unique;

std::ostream& operator<<(std::ostream& os, const Type& t) {
    t.Print(os);
    return os;
}

void UnitType::Print(std::ostream& os) const {
    os << "Unit";
}

bool UnitType::Equals(const Type* o) const {
    return dynamic_cast<const UnitType*>(o);
}

void IntType::Print(std::ostream& os) const {
    os << (signd ? 'i' : 'u') << static_cast<uint16_t>(bits);
}

bool IntType::Equals(const Type* o) const {
    auto c = dynamic_cast<const IntType*>(o);
    if (!c) return false;
    return bits == c->bits && signd == c->signd;
}

void FloatType::Print(std::ostream& os) const {
    os << 'f' << static_cast<uint16_t>(bits);
}

bool FloatType::Equals(const Type* o) const {
    auto c = dynamic_cast<const FloatType*>(o);
    if (!c) return false;
    return bits == static_cast<const FloatType*>(o)->bits;
}

void FnType::Print(std::ostream& os) const {
    if (!arg_types.empty()) {
        os << '(';
        for (auto it = arg_types.begin(), e = arg_types.end(); it != e; ++it) {
            if (it != arg_types.begin())
                os << ", ";
            os << **it;
        }
        os << ')';
    }
    os << '(';
    std::ostringstream ss;
    ss << *ret_type;
    if (!ss.str().empty()) {
        os << ' ' << ss.str();
    }
}

bool FnType::Equals(const Type* o) const {
    auto c = dynamic_cast<const FnType*>(o);
    if (!c) return false;
    if (arg_types.size() != c->arg_types.size()) return false;
    for (size_t i = 0, e = arg_types.size(); i < e; ++i)
        if (!arg_types[i]->Equals(c->arg_types[i].get()))
            return false;
    return ret_type->Equals(c->ret_type.get());
}
