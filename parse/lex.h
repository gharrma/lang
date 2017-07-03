#ifndef LEX_H
#define LEX_H
#include "parse/token.h"

namespace parse {

class Lexer {
public:
    Lexer(std::istream& is): is_(is >> std::noskipws) {}

    Token Peek();
    Token Consume();
    Token Consume(TokenKind expected);

private:
    class PositionedStream {
    public:
        PositionedStream(std::istream& is): is_(is) {}

        char Peek();

        char Get();

        template <typename Pred>
        char Get(Pred pred);

        template <typename Pred>
        size_t Get(std::string& str, Pred pred);

        void SkipWhitespace();

        int32_t Row() { return row_; }

        int32_t Col() { return col_; }

    private:
        std::istream& is_;
        char next_ = '\0';
        int32_t row_ = 1, col_ = 1;
    };

    // This is where the language-specific magic happens.
    Token GetToken();

    PositionedStream is_;
    Token buffer_;
};

struct LexError {
    std::string msg;
    Location loc;
    LexError(std::string msg, Location loc): msg(msg), loc(loc) {}
};

} // namespace parse

#endif // LEX_H
