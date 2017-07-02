#ifndef ERROR_H
#define ERROR_H
#include <string>

struct InternalCompilerError {
    std::string msg;
    InternalCompilerError(std::string msg): msg(msg) {}
};

#endif // ERROR_H
