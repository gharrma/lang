#pragma once
#include <string>
#include "location.h"

struct PositionedError {
    std::string msg;
    Loc loc;
    PositionedError(std::string msg, Loc loc): msg(msg), loc(loc) {}
};

struct LexError : PositionedError {
    LexError(std::string msg, Loc loc): PositionedError(msg, loc) {}
};

struct ParseError : PositionedError {
    ParseError(std::string msg, Loc loc): PositionedError(msg, loc) {}
};

struct TypeError : PositionedError {
    TypeError(std::string msg, Loc loc): PositionedError(msg, loc) {}
};
