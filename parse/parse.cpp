#include "parse/parse.h"
#include <unordered_map>
using std::make_unique;
using std::move;

namespace parse {

unique_ptr<Expr> Parser::ParseExpr() {
    auto lhs = ParsePrimaryExpr();
    return ParseSecondaryExpr(move(lhs), 1);
}

#define TRY_PARSE_LIT(kind, rep) \
    do if (auto token = lex_.TryGet(kind)) { \
        using Rep = decltype(token.rep); \
        return make_unique<Lit<Rep>>(token.loc, token.rep); \
    } while (0);

unique_ptr<Expr> Parser::ParsePrimaryExpr() {
    TRY_PARSE_LIT(kIntLit, int_val);
    TRY_PARSE_LIT(kFloatLit, float_val);
    TRY_PARSE_LIT(kStrLit, str_val);
    if (auto token = lex_.TryGet(kId)) {
        return make_unique<Id>(token.loc, token.str_val);
    }
    if (auto token = lex_.TryGet(kLParen)) {
        lex_.Get(kLParen);
        auto res = ParseExpr();
        lex_.Get(kRParen);
        return res;
    }
    Expected("primary expression");
}

unique_ptr<Expr> Parser::ParseSecondaryExpr(unique_ptr<Expr> lhs,
                                            int32_t min_prec) {
    while (true) {
        Token op = lex_.TryGet([min_prec](TokenKind kind) {
            return token_info.prec[kind] >= min_prec;
        });
        if (!op) return lhs;
        auto op_prec = token_info.prec[op.kind];
        auto rhs = ParsePrimaryExpr();
        if (token_info.prec[lex_.Peek().kind] > op_prec)
            rhs = ParseSecondaryExpr(move(rhs), op_prec + 1);
        lhs = make_unique<Binary>(op, move(lhs), move(rhs));
    }
}

void Parser::Expected(std::string expectation) {
    Token next = lex_.Peek();
    Location loc = next ? next.loc : lex_.CurrLoc();
    std::string msg;
    if (expectation.empty()) {
        if (next) {
            msg = BuildStr("Unexpected token \'", next, "\'.");
        } else {
            msg = "Unexpected end of input.";
        }
    } else {
        if (next) {
            msg = BuildStr("Expected ", expectation,
                           "; instead found \'", next, "\'.");
        } else {
            msg = BuildStr("Expected ", expectation,
                           "; instead found end of input.");
        }
    }
    throw ParseError(msg, loc);
}

} // namespace parse
