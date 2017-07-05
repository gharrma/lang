#ifndef PARSE_H
#define PARSE_H

#include <iostream>
#include "ast.h"
#include "lex.h"

class Parser {
public:
    Parser(Lexer& lex): lex_(lex) {}
    std::unique_ptr<Expr> ParseExpr();

private:
    std::unique_ptr<Expr> ParsePrimaryExpr();
    std::unique_ptr<Expr> ParseSecondaryExpr(std::unique_ptr<Expr> lhs,
                                             int32_t min_prec);
    [[noreturn]] void Expected(std::string expectation = "");

    Lexer& lex_;
};

#endif // PARSE_H
