#pragma once
#include <iostream>
#include "location.h"

enum TokenKind {
    kNothing = 0,

    kId,
    kIntLit,
    kFloatLit,
    kStrLit,

    kFn,
    kIf,
    kElse,

    kComma     = ',',
    kSemicolon = ';',
    kDot       = '.',
    kEq        = '=',

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

struct TokenInfo {
    static constexpr size_t range = std::numeric_limits<char>::max() + 1;
    int32_t prec[range]; // Token precedence.
    TokenInfo();
};

extern TokenInfo token_info;

struct Token {
    TokenKind kind;
    Loc loc;
    union {
        uint64_t int_val;
        double float_val;
    };
    std::string str_val;

    Token(): Token(kNothing, Loc()) {}

    Token(TokenKind kind, Loc loc)
        : kind(kind), loc(loc) {}

    Token(TokenKind kind, Loc loc, decltype(int_val) val)
        : kind(kind), loc(loc), int_val(val) {}

    Token(TokenKind kind, Loc loc, decltype(float_val) val)
        : kind(kind), loc(loc), float_val(val) {}

    Token(TokenKind kind, Loc loc, std::string val)
        : kind(kind), loc(loc), str_val(val) {}

    explicit operator bool() const { return kind != kNothing; }
};

std::ostream& operator<<(std::ostream& os, const Token& token);
