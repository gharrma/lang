#ifndef LEX_H
#define LEX_H
#include "base/logging.h"
#include "base/util.h"
#include "token.h"

namespace parse {

struct LexError {
    std::string msg;
    Location loc;
    LexError(std::string msg, Location loc): msg(msg), loc(loc) {}
};

class Lexer {
public:
    Lexer(std::istream& is): is_(is >> std::noskipws) {}

    Token Peek() {
        if (!buffer_)
            buffer_ = GetToken();
        return buffer_;
    }

    Token Get() {
        if (auto res = Peek()) {
            buffer_ = Token();
            LOG(INFO) << "[lex] " << res.loc.row << ':' << res.loc.col
                      << ' ' << res;
            return res;
        } else {
            throw LexError("Unexpected end of input.",
                           Location(is_.Row(), is_.Col()));
        }
    }

    Token Get(TokenKind expected) {
        auto res = Get();
        if (res.kind != expected)
            throw LexError(BuildStr("Expected token \'", expected, "\'",
                                    "; instead found \'", res, "\'."),
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

    Location CurrLoc() const {
        return buffer_ ? buffer_.loc : Location(is_.Row(), is_.Col());
    }

private:
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

        int32_t Row() const { return row_; }

        int32_t Col() const { return col_; }

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

} // namespace parse

#endif // LEX_H
