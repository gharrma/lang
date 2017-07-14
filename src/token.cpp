#include "token.h"

std::ostream& operator<<(std::ostream& os, const TokenKind kind) {
    // Note: when adding cases here, be sure to also add them in the lexer,
    //       especially for keywords (which cannot use an exhaustive switch).
    switch (kind) {
        case kNothing:  return os << "[invalid]";
        case kId:       return os << "[id]";
        case kIntLit:   return os << "[int]";
        case kFloatLit: return os << "[float]";
        case kStrLit:   return os << "[string]";
        case kFn:       return os << kFnStr;
        case kIf:       return os << kIfStr;
        case kElse:     return os << kElseStr;
        SINGLE_CHAR_TOKEN_CASES:
            return os << static_cast<char>(kind);
    }
}

TokenInfo::TokenInfo() {
    using Levels = std::initializer_list<std::initializer_list<TokenKind>>;
    Levels levels = {
        {kPlus, kMinus},
        {kTimes, kDiv, kMod},
    };
    size_t curr_prec = 1;
    for (auto level : levels) {
        for (auto kind : level)
            prec[kind] = curr_prec;
        ++curr_prec;
    }
}

TokenInfo token_info;

std::ostream& operator<<(std::ostream& os, const Token& token) {
    switch (token.kind) {
        case kIntLit:
            return os << token.int_val;
        case kFloatLit:
            os << token.float_val;
        case kId:
        case kStrLit:
            return os << token.str_val;
        case kNothing:
        KEYWORD_TOKEN_CASES:
        SINGLE_CHAR_TOKEN_CASES:
            return os << token.kind;
    }
}
