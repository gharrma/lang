#include "parse/token.h"

namespace parse {

std::ostream& operator<<(std::ostream& os, const TokenKind kind) {
    switch (kind) {
        case kNothing:  return os << "invalid";
        case kId:       return os << "id";
        case kIntLit:   return os << "int";
        case kFloatLit: return os << "float";
        case kStrLit:   return os << "string";
        case kIf:       return os << "if";
        case kElse:     return os << "else";
        case kDot:
        case kEq:
        case kPlus:
        case kMinus:
        case kTimes:
        case kDiv:
        case kMod:
        case kLParen:
        case kRParen:
        case kLBrace:
        case kRBrace:
        case kLBrack:
        case kRBrack:
            return os << static_cast<char>(kind);
    }
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    switch (token.kind) {
        case kId:       return os << token.str_val;
        case kIntLit:   return os << token.int_val;
        case kFloatLit: return os << token.float_val;
        case kStrLit:   return os << token.str_val;
        default:        return os << token.kind;
    }
}

} // namespace parse
