#include <iostream>
#include "base/logging.h"
#include "main/error.h"
#include "parse/lex.h"

int main() {
    using namespace parse;
    try {
        Lexer lexer(std::cin);
        while (auto t = lexer.Peek()) {
            lexer.Consume();
            LOG(INFO) << "lex " << t.loc.row << ':' << t.loc.col << ' ' << t;
        }
    } catch (LexError e) {
        LOG(FATAL) << e.loc.row << ':' << e.loc.col << ' ' << e.msg;
    } catch (InternalCompilerError e) {
        LOG(FATAL) << e.msg;
    }

    return 0;
}
