#ifndef UTIL_H
#define UTIL_H
#include <iostream>
#include <sstream>

struct InternalCompilerError {
    std::string msg;
    InternalCompilerError(std::string msg): msg(msg) {}
};

template <typename T>
static void PrintToStream(std::ostream& os, const T& arg) {
    os << arg;
}

template <typename T, typename... TT>
static void PrintToStream(std::ostream& os,
                          const T& head,
                          const TT&... tail) {
    os << head;
    PrintToStream(os, tail...);
}

template <typename... TT>
static std::string BuildStr(const TT&... args) {
    std::ostringstream ss;
    PrintToStream(ss, args...);
    return ss.str();
}

#endif // UTIL_H
