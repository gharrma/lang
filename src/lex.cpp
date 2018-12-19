#include "lex.h"

#include <cassert>
#include "util.h"

// Identifiers: [a-zA-Z][a-zA-Z0-9_']*
static bool IsIdStart(char c) { return isalpha(c); }
static bool IsIdMiddle(char c) {
    return isalpha(c) || isdigit(c) || c == '_' || c == '\'';
}

// Integers: [0-9]+
// Floats: [0-9]+[.]?[0-9]*
static bool IsNumChar(char c) { return isdigit(c) || c == '.'; }

char PositionedStream::Peek() {
    if (!buffer_)
        if (!is_.get(buffer_))
            buffer_ = '\0';
    return buffer_;
}

char PositionedStream::Get() {
    if (!buffer_)
        if (!is_.get(buffer_))
            buffer_ = '\0';
    auto res = buffer_;
    buffer_ = '\0';
    if (res == '\n') ++row_, col_ = 1;
    else if (res) ++col_;
    return res;
}

template <typename Pred>
char PositionedStream::Get(Pred pred) {
    auto ch = Peek();
    if (ch && pred(ch)) {
        auto res = Get();
        assert(res == ch);
        return res;
    } else {
        return '\0';
    }
}

template <typename Pred>
size_t PositionedStream::GetWhile(std::string& str, Pred pred) {
    size_t count = 0;
    while (auto ch = Get(pred))
        str += ch, ++count;
    return count;
}

void PositionedStream::SkipWhitespace() {
    while (Get([](char ch) { return isspace(ch); })) continue;
}

void PositionedStream::SkipLine() {
    for (char ch; (ch = Get()) && ch != '\n';) continue;
}

Token Lexer::NextToken() {
    is_.SkipWhitespace();

    auto start_row = is_.Row();
    auto start_col = is_.Col();
    auto GetLoc = [start_row, start_col, this]() {
        return Loc(start_row, start_col, is_.Row(), is_.Col());
    };

    auto ch = is_.Get();
    if (!ch) return Token();

    // Identifiers.
    if (IsIdStart(ch)) {
        std::string str(1, ch);
        is_.GetWhile(str, IsIdMiddle);
        if (str == kFnStr)   return Token(kFn,   GetLoc());
        if (str == kIfStr)   return Token(kIf,   GetLoc());
        if (str == kElseStr) return Token(kElse, GetLoc());
        if (str == kLetStr)  return Token(kLet,  GetLoc());
        return Token(kId, GetLoc(), str);
    }

    // Numbers.
    if (isdigit(ch) || (ch == '.' && isdigit(is_.Peek()))) {
        std::string str(1, ch);
        is_.GetWhile(str, IsNumChar);
        Token lit;
        size_t actual_len;
        bool decimal = str.find('.') != std::string::npos;
        try {
            if (decimal) {
                auto val = std::stod(str, &actual_len);
                lit = Token(kFloatLit, GetLoc(), val);
            } else {
                auto val = std::stoull(str, &actual_len);
                lit = Token(kIntLit, GetLoc(), val);
            }
            auto len = static_cast<size_t>(lit.loc.end_col - lit.loc.col);
            if (len != actual_len) {
                throw std::invalid_argument("Unexpected length");
            }
        }
        catch (std::invalid_argument e) {
            static const char* kMalformedIntLit = "Malformed integer literal";
            static const char* kMalformedFloatLit = "Malformed float literal";
            auto msg = decimal ? kMalformedFloatLit : kMalformedIntLit;
            throw LexError(msg, GetLoc());
        }
        catch (std::out_of_range e) {
            static const char* kIntLitRange = "Integer literal out of range";
            static const char* kFloatLitRange = "Float literal out of range";
            auto msg = decimal ? kFloatLitRange : kIntLitRange;
            throw LexError(msg, GetLoc());
        }
        return lit;
    }

    // Lex single-character tokens with an exhaustive switch to avoid bugs.
    auto as_single_char_token = static_cast<TokenKind>(ch);
    switch (as_single_char_token) {
        SINGLE_CHAR_TOKEN_CASES:
            return Token(as_single_char_token, GetLoc());
        case kNothing:
        case kId:
        case kIntLit:
        case kFloatLit:
        case kStrLit:
        KEYWORD_TOKEN_CASES:
            break;
    }

    throw LexError(BuildStr("Unexpected character \'", ch, "\'"),
                   GetLoc());
}
