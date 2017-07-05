#include "parse.h"

#include <unordered_map>
#include "error.h"
#include "llvm/ADT/STLExtras.h"

std::unique_ptr<Expr> Parser::ParseExpr() {
    auto lhs = ParsePrimaryExpr();
    auto res = ParseSecondaryExpr(std::move(lhs), 1);
    return res;
}

#define TRY_PARSE_LIT(kind, rep) \
    do if (auto token = lex_.TryGet(kind)) { \
        using Rep = decltype(token.rep); \
        return llvm::make_unique<Lit<Rep>>(token.loc, token.rep); \
    } while (0);

std::unique_ptr<Expr> Parser::ParsePrimaryExpr() {
    TRY_PARSE_LIT(kIntLit, int_val);
    TRY_PARSE_LIT(kFloatLit, float_val);
    TRY_PARSE_LIT(kStrLit, str_val);
    if (auto token = lex_.TryGet(kId)) {
        return llvm::make_unique<Id>(token.loc, token.str_val);
    }
    if (auto token = lex_.TryGet(kLParen)) {
        auto res = ParseExpr();
        lex_.Get(kRParen);
        return res;
    }
    Expected("primary expression");
}

std::unique_ptr<Expr> Parser::ParseSecondaryExpr(std::unique_ptr<Expr> lhs,
                                                  int32_t min_prec) {
    while (true) {
        Token op = lex_.TryGet([min_prec](TokenKind kind) {
            return token_info.prec[kind] >= min_prec;
        });
        if (!op) return lhs;
        auto op_prec = token_info.prec[op.kind];
        auto rhs = ParsePrimaryExpr();
        if (token_info.prec[lex_.Peek().kind] > op_prec)
            rhs = ParseSecondaryExpr(std::move(rhs), op_prec + 1);
        lhs = llvm::make_unique<Binary>(op, std::move(lhs), std::move(rhs));
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
            msg = BuildStr("Unexpected token \'", next, "\'.");
        } else {
            msg = BuildStr("Expected ", expectation, '.');
        }
    }
    throw ParseError(msg, loc);
}
