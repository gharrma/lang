#include "parse.h"

#include <unordered_map>
#include "llvm/ADT/STLExtras.h"
#include "error.h"

using namespace llvm;
using std::unique_ptr;
using llvm::make_unique;
using std::make_shared;
using std::move;

// expr | fn
unique_ptr<Node> Parser::ParseTopLevelConstruct() {
    if (lex_.Peek(kFn))
        return ParseFnDecl();
    return ParseExpr();
}

// TODO: Split this up.
// expr | let x = expr
unique_ptr<Expr> Parser::ParseExpr() {
    if (lex_.TryGet(kLet)) {
        auto name = lex_.Get(kId);
        lex_.Get(kEq);
        auto expr = ParseExpr();
        return make_unique<LocalVarDecl>(name, std::move(expr));
    }
    else {
        auto lhs = ParsePrimaryExpr();
        auto res = ParseSecondaryExpr(move(lhs), 1);
        return res;
    }
}

// fn id(arg1 type1, ...) ret_type = expr
unique_ptr<FnDecl> Parser::ParseFnDecl() {
    auto fn_start_loc = lex_.Get(kFn).loc;
    auto name = lex_.Get(kId);
    auto proto_loc = lex_.CurrLoc();
    std::vector<unique_ptr<Param>> arg_types;
    if (lex_.TryGet(kLParen)) {
        if (lex_.Peek(kLParen))
            throw ParseError(
                "Functions without arguments should not have parentheses",
                lex_.CurrLoc());
        do {
            auto param = lex_.Get(kId);
            auto type = ParseType();
            arg_types.push_back(make_unique<Param>(param, move(type)));
        } while (lex_.TryGet(kComma));
        lex_.Get(kRParen);
    }
    auto ret_type = lex_.Peek(kId)
        ? make_unique<ParsedType>(lex_.Get(kId))
        : make_unique<ParsedType>(lex_.CurrLoc(), make_shared<UnitType>());
    auto proto = make_unique<FnProto>(
        proto_loc, move(arg_types), move(ret_type));
    lex_.Get(kEq);
    auto body = ParseExpr();
    return make_unique<FnDecl>(fn_start_loc, name, move(proto), move(body));
}

#define TRY_PARSE(token_kind, node, ...) \
    do if (auto token = lex_.TryGet(token_kind)) \
        return make_unique<node>(__VA_ARGS__); \
    while (0)

// int | float | id | str | (expr) | id(arg1, ...) | {...}
unique_ptr<Expr> Parser::ParsePrimaryExpr() {
    if (auto int_lit = lex_.TryGet(kIntLit)) {
        return make_unique<IntLit>(int_lit.loc, int_lit.int_val);
    }
    if (auto float_lit = lex_.TryGet(kFloatLit)) {
        return make_unique<FloatLit>(float_lit.loc, float_lit.float_val);
    }

    if (auto name = lex_.TryGet(kId)) {
        if (lex_.TryGet(kLParen)) {
            std::vector<unique_ptr<Expr>> args;
            if (!lex_.TryGet(kRParen)) {
                do { args.push_back(ParseExpr()); }
                while (lex_.TryGet(kComma));
                lex_.Get(kRParen);
            }
            return make_unique<Call>(
                Loc(name.loc, lex_.CurrLoc()), name, std::move(args));
        } else {
            return make_unique<Id>(name);
        }
    }

    if (auto lbrace = lex_.TryGet(kLBrace)) {
        std::vector<std::unique_ptr<Expr>> exprs;
        while (!lex_.Peek(kRBrace))
            exprs.push_back(ParseExpr());
        auto rbrace = lex_.Get(kRBrace);
        return make_unique<Block>(Loc(lbrace.loc, rbrace.loc),
                                  std::move(exprs));
    }

    if (auto lparen = lex_.TryGet(kLParen)) {
        auto res = ParseExpr();
        auto rparen = lex_.TryGet(kRParen);
        if (!rparen) Expected("\')\'");
        res->loc = Loc(lparen.loc, rparen.loc);
        return res;
    }

    Expected("primary expression");
}

// expr op expr
unique_ptr<Expr> Parser::ParseSecondaryExpr(unique_ptr<Expr> lhs,
                                            int32_t min_prec) {
    while (true) {
        Token op = lex_.TryGet([min_prec](TokenKind kind) {
            return token_info.prec[kind] >= min_prec;
        });
        if (!op) return lhs;
        auto op_prec = token_info.prec[op.kind];
        auto rhs = ParsePrimaryExpr();
        auto next_op_prec = token_info.prec[lex_.Peek().kind];
        if (next_op_prec > op_prec)
            rhs = ParseSecondaryExpr(move(rhs), op_prec + 1);
        lhs = make_unique<Binary>(op, move(lhs), move(rhs));
    }
}

unique_ptr<ParsedType> Parser::ParseType() {
    return make_unique<ParsedType>(lex_.Get(kId));
}

void Parser::Expected(std::string expectation) {
    Token next = lex_.Peek();
    Loc loc = next ? next.loc : lex_.CurrLoc();
    std::string msg;
    if (expectation.empty()) {
        msg = next
            ? msg = BuildStr("Unexpected token \'", next, "\'")
            : msg = "Unexpected end of input";
    } else {
        msg = next
            ? msg = BuildStr("Unexpected token \'", next, "\'")
            : msg = BuildStr("Expected ", expectation);
    }
    throw ParseError(msg, loc);
}
