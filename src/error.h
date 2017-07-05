#ifndef MAIN_ERROR_H
#define MAIN_ERROR_H
#include "parse/location.h"

using parse::Location;

struct InternalCompilerError {
    std::string msg;
    InternalCompilerError(std::string msg): msg(msg) {}
};

struct LexError {
    std::string msg;
    Location loc;
    LexError(std::string msg, Location loc): msg(msg), loc(loc) {}
};

struct ParseError {
    std::string msg;
    Location loc;
    ParseError(std::string msg, Location loc): msg(msg), loc(loc) {}
};

struct TypeError {
    std::string msg;
    Location loc;
    TypeError(std::string msg, Location loc): msg(msg), loc(loc) {}
};

#endif // MAIN_ERROR_H
