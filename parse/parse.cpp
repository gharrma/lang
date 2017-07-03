#include "parse/parse.h"
#include <unordered_map>
using std::make_unique;
using std::move;

namespace parse {

std::unordered_map<TokenKind, int32_t> prec_map({
    {kPlus, 1}, {kMinus, 1},
    {kTimes, 2}, {kDiv, 2}, {kMod, 2},
});

unique_ptr<Expr> Parser::ParseExpr() {
    auto lhs = ParsePrimaryExpr();
    return ParseSecondaryExpr(move(lhs), 1);
}

unique_ptr<Expr> Parser::ParsePrimaryExpr() {
    if (auto token = lex_.TryGet(kId)) {
        return make_unique<Id>(token.loc, token.str_val);
    }
    if (auto token = lex_.TryGet(kIntLit)) {
        using Rep = decltype(token.int_val);
        return make_unique<Lit<Rep>>(token.loc, token.int_val);
    }
    if (auto token = lex_.TryGet(kFloatLit)) {
        using Rep = decltype(token.float_val);
        return make_unique<Lit<Rep>>(token.loc, token.float_val);
    }
    if (auto token = lex_.TryGet(kStrLit)) {
        using Rep = decltype(token.str_val);
        return make_unique<Lit<Rep>>(token.loc, token.str_val);
    }
    if (auto token = lex_.TryGet(kLParen)) {
        lex_.Get(kLParen);
        auto res = ParseExpr();
        lex_.Get(kRParen);
        return res;
    }
    Unexpected(lex_.Peek(), "primary expression");
}

unique_ptr<Expr> Parser::ParseSecondaryExpr(unique_ptr<Expr> lhs,
                                            int32_t min_prec) {
    while (true) {
        Token op = lex_.TryGet([min_prec](TokenKind kind) {
            return prec_map[kind] >= min_prec;
        });
        if (!op) return lhs;
        auto op_prec = prec_map[op.kind];
        auto rhs = ParsePrimaryExpr();
        if (prec_map[lex_.Peek().kind] > op_prec)
            rhs = ParseSecondaryExpr(move(rhs), op_prec + 1);
        lhs = make_unique<Binary>(op, move(lhs), move(rhs));
    }
}

void Parser::Unexpected(Token token, std::string expectation) {
    std::string msg;
    if (expectation.empty()) {
        msg = BuildStr("Unexpected token \'", token, "\'");
    } else {
        msg = BuildStr("Expected ", expectation,
                       "; instead found \'", token, "\'");
    }
    throw ParseError(msg, token.loc);
}

} // namespace parse
