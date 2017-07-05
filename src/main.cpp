#include <iostream>
#include <fstream>
#include "base/logging.h"
#include "base/util.h"
#include "parse/lex.h"
#include "parse/parse.h"
#include "typecheck/types.h"

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
        lexer.SkipLine(); \
    } while (0)

int main(int argc, char* argv[]) {
    using namespace parse;
    using namespace typecheck;

    if (argc > 1) {
        // Read from files.
        int i = 1;
        try {
            for (; i < argc; ++i) {
                std::ifstream file(argv[i]);
                if (!file)
                    LOG(FATAL) << BuildStr("Could not open file ", argv[i]);
                Lexer lexer(file);
                Parser parser(lexer);
                auto expr = parser.ParseExpr();
                expr->PrettyPrint(std::cout);
                std::cout << std::endl;
            }
        }
        catch (const LexError& e)   { FILE_ERROR(lex); }
        catch (const ParseError& e) { FILE_ERROR(parse); }
        catch (const TypeError& e)  { FILE_ERROR(typecheck); }
        catch (const InternalCompilerError& e) { LOG(FATAL) << e.msg; }
    }
    else {
        // REPL.
        Lexer lexer(std::cin);
        Parser parser(lexer);
        while (true) {
            try {
                std::cout << "c> ";
                auto expr = parser.ParseExpr();
                lexer.Get(parse::kSemicolon);
                std::cout << "[parse] ";
                expr->PrettyPrint(std::cout);
                std::cout << std::endl;
                auto value = expr->Eval();
                std::cout << "[value] " << value << std::endl;
            }
            catch (const LexError& e)   { REPL_ERROR(lex); }
            catch (const ParseError& e) { REPL_ERROR(parse); }
            catch (const TypeError& e)  { REPL_ERROR(typecheck); }
            catch (const InternalCompilerError& e) { LOG(FATAL) << e.msg; }
        }
    }

    return 0;
}
