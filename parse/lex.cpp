#include <cassert>
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

// Integers: [0-9]+
// Floats: [0-9]*[.]?[0-9]*
static bool IsNumChar(char c) { return isdigit(c) || c == '.'; }

char PeekStream::Peek() {
    if (!next_)
        if (!is_.get(next_))
            next_ = '\0';
    return next_;
}

char PeekStream::Get() {
    if (!next_)
        if (!is_.get(next_))
            next_ = '\0';
    auto res = next_;
    next_ = '\0';
    return res;
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

char Lexer::GetChar() {
    auto ch = is_.Get();
    if (ch == '\n') ++row_, col_ = 1;
    else if (ch) ++col_;
    return ch;
}

template <typename Pred>
char Lexer::GetChar(Pred pred) {
    auto ch = PeekChar();
    if (ch && pred(ch)) {
        auto res = GetChar();
        assert(res == ch);
        return res;
    } else {
        return '\0';
    }
}

template <typename Pred>
size_t Lexer::GetChars(std::string& str, Pred pred) {
    size_t count = 0;
    while (auto ch = GetChar(pred))
        str += ch, ++count;
    return count;
}

void Lexer::SkipWhitespace() {
    while (GetChar([](char ch) { return isspace(ch); }));
}

Token Lexer::GetToken() {
    SkipWhitespace();

    auto GetLocation = [start_row = row_, start_col = col_, this]() {
        return Location(start_row, start_col, row_, col_);
    };

    auto ch = GetChar();
    if (!ch) return Token();

    // Identifiers.
    if (IsIdStart(ch)) {
        std::string str(1, ch);
        GetChars(str, IsIdMiddle);
        return Token(kId, GetLocation(), str);
    }

    // Numbers.
    if (isdigit(ch) || (ch == '.' && isdigit(PeekChar()))) {
        std::string str(1, ch);
        GetChars(str, IsNumChar);
        Token lit;
        size_t actual_len;
        bool decimal = str.find('.') != std::string::npos;
        try {
            if (decimal) {
                auto val = std::stod(str, &actual_len);
                lit = Token(kFloatLit, GetLocation(), val);
            } else {
                auto val = std::stoull(str, &actual_len);
                lit = Token(kIntLit, GetLocation(), val);
            }
            if (lit.loc.end_col - lit.loc.col != actual_len) {
                throw std::invalid_argument("Unexpected length");
            }
        }
        catch (std::invalid_argument e) {
            auto msg = decimal ? kMalformedFloatLit : kMalformedIntLit;
            throw LexError(msg, GetLocation());
        }
        catch (std::out_of_range e) {
            auto msg = decimal ? kFloatLitOutOfRange : kIntLitOutOfRange;
            throw LexError(msg, GetLocation());
        }
        return lit;
    }

    switch (ch) {
        case '.': case '=':
        case '+': case '-': case '*': case '/': case '%':
        case '(': case ')': case '{': case '}': case '[': case ']':
            return Token(static_cast<TokenKind>(ch), GetLocation());
        default:
            throw LexError(BuildStr("Unexpected character ", ch),
                           GetLocation());
    }
}

} // namespace parse
