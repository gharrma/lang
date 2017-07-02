#include "base/logging.h"
#include "base/util.h"
#include "main/error.h"
#include "parse/lex.h"

namespace parse {

// Identifiers: [a-zA-Z][a-zA-Z0-9_']*
static bool IsIdStart(char c) { return isalpha(c); }
static bool IsIdMiddle(char c) {
    return isalpha(c) || isdigit(c) || c == '_' || c == '\'';
}

Token Lexer::Peek() {
    if (!buffer_)
        buffer_ = GetToken();
    return buffer_;
}

Token Lexer::Consume() {
    if (auto res = Peek()) {
        buffer_ = Token();
        return res;
    } else {
        throw LexError("Unexpected end of input", Location(row_, col_));
    }
}

Token Lexer::Consume(TokenKind expected) {
    auto res = Consume();
    if (res.kind != expected)
        throw LexError(BuildStr("Expected token ", expected,
                                "; instead found ", res),
                       res.loc);
    return res;
}

bool Lexer::GetChar(char& ch) {
    do {
        if (!is_.get(ch)) return false;
        if (ch == '\n') ++row_, col_ = 1;
        else ++col_;
    } while (isspace(ch));
    return true;
}

template <typename Pred>
bool Lexer::GetChar(char& ch, Pred pred) {
    if (!GetChar(ch)) {
        return false;
    } else if (pred(ch)) {
        return true;
    } else {
        --col_;
        if (!is_.putback(ch))
            throw InternalCompilerError(
                "Failed to put back character while lexing");
        return false;
    }
}

template <typename Pred>
size_t Lexer::GetChars(std::string& str, Pred pred) {
    size_t count = 0;
    char ch;
    while (GetChar(ch, pred))
        str += ch, ++count;
    return count;
}

bool Lexer::TryConsumeChar(char expected) {
    char ignore;
    return GetChar(ignore, [=](char ch) { return ch == expected; });
}

Token Lexer::GetToken() {
    auto GetLocation = [start_row = row_, start_col = col_, this]() {
        return Location(start_row, start_col, row_, col_);
    };

    char ch;
    if (!GetChar(ch)) {
        return Token();
    }

    // Identifiers.
    if (IsIdStart(ch)) {
        std::string str(1, ch);
        GetChars(str, IsIdMiddle);
        return Token(kId, GetLocation(), str);
    }

    switch (ch) {
        case '+': case '-': case '*': case '/': case '%':
        case '(': case ')': case '{': case '}': case '[': case ']':
            return Token(static_cast<TokenKind>(ch), GetLocation());
        default:
            throw LexError(BuildStr("Unexpected character ", ch),
                           GetLocation());
    }
}

} // namespace parse
