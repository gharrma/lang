#ifndef LEX_H
#define LEX_H
#include "parse/token.h"

namespace parse {

class PeekStream {
public:
    PeekStream(std::istream& is): is_(is) {}

    char Peek();
    char Get();

private:
    std::istream& is_;
    char next_ = '\0';
};

class Lexer {
public:
    Lexer(std::istream& is): is_(is >> std::noskipws) {}

    Token Peek();
    Token Consume();
    Token Consume(TokenKind expected);

private:
    char GetChar();

    char PeekChar() { return is_.Peek(); }

    template <typename Pred>
    char GetChar(Pred pred);

    template <typename Pred>
    size_t GetChars(std::string& str, Pred pred);

    void SkipWhitespace();

    // This is where the language-specific magic happens.
    Token GetToken();

    PeekStream is_;
    Token buffer_;
    int32_t row_ = 1, col_ = 1;
};

struct LexError {
    std::string msg;
    Location loc;
    LexError(std::string msg, Location loc)
        : msg(msg), loc(loc) {}
};

} // namespace parse

#endif // LEX_H
