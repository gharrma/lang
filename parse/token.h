#ifndef TOKEN_H
#define TOKEN_H
#include <iostream>
#include "parse/location.h"

namespace parse {

enum TokenKind {
    kNothing = 0,

    kId,
    kIntLit,
    kFloatLit,
    kStrLit,

    kIf,
    kElse,

    kDot = '.',
    kEq  = '=',

    kPlus  = '+',
    kMinus = '-',
    kTimes = '*',
    kDiv   = '/',
    kMod   = '%',

    kLParen = '(',
    kRParen = ')',
    kLBrace = '{',
    kRBrace = '}',
    kLBrack = '[',
    kRBrack = ']',
};

std::ostream& operator<<(std::ostream& os, const TokenKind kind);

struct Token {
    TokenKind kind;
    Location loc;
    uint64_t int_val;
    double float_val;
    std::string str_val;

    Token(): Token(kNothing, Location()) {}

    Token(TokenKind kind, Location loc)
        : kind(kind), loc(loc) {}

    Token(TokenKind kind, Location loc, uint64_t val)
        : kind(kind), loc(loc), int_val(val) {}

    Token(TokenKind kind, Location loc, double val)
        : kind(kind), loc(loc), float_val(val) {}

    Token(TokenKind kind, Location loc, std::string val)
        : kind(kind), loc(loc), str_val(val) {}

    explicit operator bool() const { return kind != kNothing; }
};

std::ostream& operator<<(std::ostream& os, const Token& token);

} // namespace parse

#endif // TOKEN_H
