#ifndef LEX_H
#define LEX_H
#include "parse/token.h"

namespace parse {

class Lexer {
public:
    Token Peek();
    Token Consume();
    Token Consume(TokenKind expected);

    Lexer(std::istream& is): is_(is >> std::noskipws) {}

private:
    bool GetChar(char& ch);

    template <typename Pred>
    bool GetChar(char& ch, Pred pred);

    template <typename Pred>
    size_t GetChars(std::string& str, Pred pred);

    bool TryConsumeChar(char expected);

    // This is where the language-specific magic happens.
    Token GetToken();

    std::istream& is_;
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
