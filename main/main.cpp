#include <iostream>
#include "base/logging.h"
#include "base/util.h"
#include "parse/lex.h"
#include "parse/parse.h"
#include "typecheck/types.h"

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
        catch (LexError e) {
            std::cerr << e.loc.row << ':' << e.loc.col << " [lex] "
                      << e.msg << std::endl;
        }
        catch (ParseError e) {
            std::cerr << e.loc.row << ':' << e.loc.col << " [parse] "
                      << e.msg << std::endl;
        }
        catch (TypeError e) {
            std::cerr << e.loc.row << ':' << e.loc.col << " [typecheck] "
                      << e.msg << std::endl;
        }
        catch (InternalCompilerError e) {
            LOG(FATAL) << e.msg;
        }
    }
    return 0;
}
