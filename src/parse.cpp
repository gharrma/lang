#include "parse.h"

#include <unordered_map>
#include "llvm/ADT/STLExtras.h"
#include "error.h"

using namespace llvm;
using std::move;

std::unique_ptr<Expr> Parser::ParseExpr() {
    auto lhs = ParsePrimaryExpr();
    auto res = ParseSecondaryExpr(move(lhs), 1);
    return res;
}

#define TRY_PARSE(token_kind, node, ...) \
    do if (auto token = lex_.TryGet(token_kind)) \
        return make_unique<node>(__VA_ARGS__); \
    while (0)

std::unique_ptr<Expr> Parser::ParsePrimaryExpr() {
    TRY_PARSE(kIntLit, IntLit, token.loc, token.int_val);
    TRY_PARSE(kFloatLit, FloatLit, token.loc, token.float_val);
    TRY_PARSE(kId, Id, token.loc, token.str_val);
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
            rhs = ParseSecondaryExpr(move(rhs), op_prec + 1);
        lhs = make_unique<Binary>(op, move(lhs), move(rhs));
    }
}

void Parser::Expected(std::string expectation) {
    Token next = lex_.Peek();
    Loc loc = next ? next.loc : lex_.CurrLoc();
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
