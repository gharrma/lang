#include <iostream>
#include "base/logging.h"
#include "base/util.h"
#include "parse/lex.h"
#include "parse/parse.h"
#include "typecheck/types.h"

int main() {
    using namespace parse;
    using namespace typecheck;
    try {
        Lexer lexer(std::cin);
        Parser parser(lexer);
        while (true) {
            std::cout << "c> ";
            auto expr = parser.ParseExpr();
            lexer.Get(parse::kSemicolon);
            std::cout << "[parse] ";
            expr->PrettyPrint(std::cout);
            std::cout << std::endl;
            std::cout << "[value] " << expr->Eval() << std::endl;
        }
    }
    catch (LexError e) {
        LOG(FATAL) << "[lex] " << e.loc.row << ':' << e.loc.col
                   << ' ' << e.msg;
    }
    catch (ParseError e) {
        LOG(FATAL) << "[parse] " << e.loc.row << ':' << e.loc.col
                   << ' ' << e.msg;
    }
    catch (TypeError e) {
        LOG(FATAL) << "[typecheck] " << e.loc.row << ':' << e.loc.col
                   << ' ' << e.msg;
    }
    catch (InternalCompilerError e) {
        LOG(FATAL) << e.msg;
    }

    return 0;
}
