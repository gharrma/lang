#include "type.h"

#include <sstream>
#include <vector>
#include "llvm/ADT/STLExtras.h"
#include "ast.h"
#include "error.h"
#include "util.h"
#include "visit.h"

using llvm::make_unique;

std::ostream& operator<<(std::ostream& os, const Type* t) {
    t->Print(os);
    return os;
}

void UnitType::Print(std::ostream& os) const {
    os << "Unit";
}

bool UnitType::Equals(const Type* o) const {
    return dynamic_cast<const UnitType*>(o);
}

void IntType::Print(std::ostream& os) const {
    os << (signd ? 'i' : 'u') << bits;
}

bool IntType::Equals(const Type* o) const {
    auto c = dynamic_cast<const IntType*>(o);
    if (!c) return false;
    return bits == c->bits && signd == c->signd;
}

void FloatType::Print(std::ostream& os) const { os << 'f' << bits; }

bool FloatType::Equals(const Type* o) const {
    auto c = dynamic_cast<const FloatType*>(o);
    if (!c) return false;
    return bits == static_cast<const FloatType*>(o)->bits;
}
