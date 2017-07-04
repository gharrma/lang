#include <iostream>
#include "base/logging.h"
#include "base/util.h"
#include "parse/lex.h"
#include "parse/parse.h"
#include "typecheck/types.h"

#define PROCESS_ERROR(kind) \
    do { \
        std::cerr << e.loc.row << ':' << e.loc.col << " [" #kind "] " \
                  << e.msg << std::endl; \
        lexer.SkipLine(); \
    } while (0)

int main() {
    using namespace parse;
    using namespace typecheck;
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
        catch (LexError e)   { PROCESS_ERROR(lex); }
        catch (ParseError e) { PROCESS_ERROR(parse); }
        catch (TypeError e)  { PROCESS_ERROR(typecheck); }
        catch (InternalCompilerError e) {
            LOG(FATAL) << e.msg;
        }
    }
    return 0;
}
