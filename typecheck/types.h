#ifndef TYPES_H
#define TYPES_H
#include <string>
#include "parse/location.h"
using parse::Location;

namespace typecheck {

struct TypeError {
    std::string msg;
    Location loc;
    TypeError(std::string msg, Location loc): msg(msg), loc(loc) {}
};

} // namespace typecheck

#endif // TYPES_H
