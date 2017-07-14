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

// Helps for exhaustive switches.
#define KEYWORD_TOKEN_CASES \
    case kFn: \
    case kIf: \
    case kElse

// Helps for exhaustive switches.
#define SINGLE_CHAR_TOKEN_CASES \
    case kComma: \
    case kDot: \
    case kEq: \
    case kSemicolon: \
    case kPlus: \
    case kMinus: \
    case kTimes: \
    case kDiv: \
    case kMod: \
    case kLParen: \
    case kRParen: \
    case kLBrace: \
    case kRBrace: \
    case kLBrack: \
    case kRBrack

static const char* kFnStr = "fn";
static const char* kIfStr = "if";
static const char* kElseStr = "else";

std::ostream& operator<<(std::ostream& os, const TokenKind kind);

struct TokenInfo {
    static constexpr size_t range = std::numeric_limits<char>::max() + 1;
    int32_t prec[range]; // Token precedence.
    TokenInfo();
};

extern TokenInfo token_info;

using IntLitRep = uint64_t;
using FloatLitRep = double;

struct Token {
    TokenKind kind;
    Loc loc;
    union {
        IntLitRep int_val;
        FloatLitRep float_val;
    };
    std::string str_val;

    Token(): Token(kNothing, Loc()) {}

    Token(TokenKind kind, Loc loc)
        : kind(kind), loc(loc) {}

    Token(TokenKind kind, Loc loc, IntLitRep val)
        : kind(kind), loc(loc), int_val(val) {}

    Token(TokenKind kind, Loc loc, FloatLitRep val)
        : kind(kind), loc(loc), float_val(val) {}

    Token(TokenKind kind, Loc loc, std::string val)
        : kind(kind), loc(loc), str_val(val) {}

    explicit operator bool() const { return kind != kNothing; }
};

std::ostream& operator<<(std::ostream& os, const Token& token);
