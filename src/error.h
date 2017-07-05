#ifndef ERROR_H
#define ERROR_H

#include "location.h"

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

#endif // ERROR_H
