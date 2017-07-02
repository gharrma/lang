#ifndef ERROR_H
#define ERROR_H
#include <string>

struct InternalCompilerError {
    std::string msg;
    InternalCompilerError(std::string msg): msg(msg) {}
};

static const char* kMalformedIntLit = "Malformed integer literal";
static const char* kMalformedFloatLit = "Malformed float literal";
static const char* kIntLitOutOfRange = "Integer literal out of range";
static const char* kFloatLitOutOfRange = "Float literal out of range";

#endif // ERROR_H
