#pragma once
#include <string>
#include "location.h"

struct LexError {
    std::string msg;
    Loc loc;
    LexError(std::string msg, Loc loc): msg(msg), loc(loc) {}
};

struct ParseError {
    std::string msg;
    Loc loc;
    ParseError(std::string msg, Loc loc): msg(msg), loc(loc) {}
};

struct TypeError {
    std::string msg;
    Loc loc;
    TypeError(std::string msg, Loc loc): msg(msg), loc(loc) {}
};
