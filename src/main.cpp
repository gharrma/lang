#include <iostream>
#include <fstream>
#include "emit.h"
#include "lex.h"
#include "parse.h"
#include "util.h"
#include "typecheck.h"

#define FILE_ERROR(kind) \
    do { \
        std::cerr << argv[i] << ':' \
                  << e.loc.row << ':' << e.loc.col << " [" #kind "] " \
                  << e.msg << std::endl; \
    } while (0)

#define REPL_ERROR(kind) \
    do { \
        std::cerr << e.loc.row << ':' << e.loc.col << " [" #kind "] " \
                  << e.msg << std::endl; \
    } while (0)

int main(int argc, char* argv[]) {
    if (argc == 2 && strcmp("--lex", argv[1]) == 0) {
        // Lex from stdin.
        Lexer lexer(std::cin);
        std::ostringstream ss;
        while (auto token = lexer.Get()) {
            ss << token;
            if (token.kind == kSemicolon) {
                std::cout << ss.str() << std::endl;
                ss = std::ostringstream();
            } else {
                ss << ' ';
            }
        }
    }
    else if (argc > 1) {
        // Parse from files.
        int i = 1;
        try {
            for (; i < argc; ++i) {
                std::ifstream file(argv[i]);
                if (!file)
                    LOG(FATAL) << BuildStr("Could not open file ", argv[i]);
                Lexer lexer(file);
                Parser parser(lexer);
                auto expr = parser.ParseExpr();
                std::cout << *expr << std::endl;
            }
        }
        catch (const LexError& e)   { FILE_ERROR(lex); }
        catch (const ParseError& e) { FILE_ERROR(parse); }
        catch (const TypeError& e)  { FILE_ERROR(typecheck); }
    }
    else {
        // REPL.
        Lexer lexer(std::cin);
        Parser parser(lexer);
        EmitContext context;
        while (true) {
            try {
                std::cout << "c> ";
                auto expr = parser.ParseExpr();
                lexer.Get(kSemicolon);
                std::cout << "[parse] ";
                std::cout << *expr << std::endl;
                auto type_errors = TypeCheck(*expr);
                for (const auto& e : type_errors)
                    REPL_ERROR(typecheck);
                if (!type_errors.empty())
                    continue;
                auto ir = expr->Emit(context);
                std::cout << "[value] " << std::flush;
                ir->dump();
            }
            catch (const LexError& e) {
                REPL_ERROR(lex);
                lexer.SkipLine();
            }
            catch (const ParseError& e) {
                REPL_ERROR(parse);
                lexer.SkipLine();
            }
        }
    }
    return 0;
}
