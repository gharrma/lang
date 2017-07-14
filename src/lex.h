#pragma once
#include "util.h"
#include "error.h"
#include "token.h"

// Reads characters, keeps a lookahead buffer,
// and tracks the current row and column within a stream.
//
// Methods starting with `Get` will return a null character if there's
// nothing to get (e.g., end of file or predicate not satisfied).
class PositionedStream {
public:
    PositionedStream(std::istream& is): is_(is) {}

    char Peek();

    char Get();

    template <typename Pred>
    char Get(Pred pred);

    template <typename Pred>
    size_t GetWhile(std::string& str, Pred pred);

    void SkipWhitespace();

    void SkipLine();

    int32_t Row() const { return row_; }

    int32_t Col() const { return col_; }

private:
    std::istream& is_;
    char buffer_ = '\0';
    int32_t row_ = 1, col_ = 1;
};

// Converts a stream of characters into a stream of tokens with
// a single lookahead buffer.
class Lexer {
public:
    Lexer(std::istream& is): is_(is >> std::noskipws) {}

    Token Peek() {
        if (!buffer_)
            buffer_ = NextToken();
        return buffer_;
    }

    bool Peek(TokenKind expected) {
        return Peek().kind == expected;
    }

    // Throws a parse error if no tokens are available.
    Token Get() {
        if (auto res = Peek()) {
            buffer_ = Token();
            return res;
        } else {
            throw ParseError("Unexpected end of input",
                             Loc(is_.Row(), is_.Col()));
        }
    }

    Token Get(TokenKind expected) {
        auto res = Get();
        if (res.kind != expected)
            throw ParseError(BuildStr("Unexpected token \'", res, "\'"),
                             res.loc);
        return res;
    }

    template <typename Pred>
    Token TryGet(Pred pred) {
        if (pred(Peek().kind))
            return Get();
        return Token();
    }

    Token TryGet(TokenKind expected) {
        return TryGet([=](TokenKind kind) { return kind == expected; });
    }

    void SkipLine() { is_.SkipLine(); buffer_ = Token(); }

    Loc CurrLoc() const {
        return buffer_ ? buffer_.loc : Loc(is_.Row(), is_.Col());
    }

private:
    // This is where the language-specific magic happens.
    Token NextToken();

    PositionedStream is_;
    Token buffer_;
};
