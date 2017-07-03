#include <cassert>
#include "base/logging.h"
#include "base/util.h"
#include "parse/lex.h"

namespace parse {

// Identifiers: [a-zA-Z][a-zA-Z0-9_']*
static bool IsIdStart(char c) { return isalpha(c); }
static bool IsIdMiddle(char c) {
    return isalpha(c) || isdigit(c) || c == '_' || c == '\'';
}

// Integers: [0-9]+
// Floats: [0-9]+[.]?[0-9]*
static bool IsNumChar(char c) { return isdigit(c) || c == '.'; }

char Lexer::PositionedStream::Peek() {
    if (!next_)
        if (!is_.get(next_))
            next_ = '\0';
    return next_;
}

char Lexer::PositionedStream::Get() {
    if (!next_)
        if (!is_.get(next_))
            next_ = '\0';
    auto res = next_;
    next_ = '\0';
    if (res == '\n') ++row_, col_ = 1;
    else if (res) ++col_;
    return res;
}

template <typename Pred>
char Lexer::PositionedStream::Get(Pred pred) {
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
size_t Lexer::PositionedStream::GetWhile(std::string& str, Pred pred) {
    size_t count = 0;
    while (auto ch = Get(pred))
        str += ch, ++count;
    return count;
}

void Lexer::PositionedStream::SkipWhitespace() {
    while (Get([](char ch) { return isspace(ch); }));
}

Token Lexer::GetToken() {
    is_.SkipWhitespace();

    auto GetLocation = [start_row = is_.Row(),
                        start_col = is_.Col(),
                        this]() {
        return Location(start_row, start_col, is_.Row(), is_.Col());
    };

    auto ch = is_.Get();
    if (!ch) return Token();

    // Identifiers.
    if (IsIdStart(ch)) {
        std::string str(1, ch);
        is_.GetWhile(str, IsIdMiddle);
        return Token(kId, GetLocation(), str);
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
            static const char* kMalformedIntLit = "Malformed integer literal.";
            static const char* kMalformedFloatLit = "Malformed float literal.";
            auto msg = decimal ? kMalformedFloatLit : kMalformedIntLit;
            throw LexError(msg, GetLocation());
        }
        catch (std::out_of_range e) {
            static const char* kIntLitRange = "Integer literal out of range.";
            static const char* kFloatLitRange = "Float literal out of range.";
            auto msg = decimal ? kFloatLitRange : kIntLitRange;
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
            throw LexError(BuildStr("Unexpected character \'", ch, "\'."),
                           GetLocation());
    }
}

} // namespace parse
