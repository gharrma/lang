#include <iostream>
#include "base/logging.h"
#include "main/error.h"
#include "parse/lex.h"
#include "parse/parse.h"

int main() {
    using namespace parse;
    try {
        Parser parser(std::cin);
        auto expr = parser.ParseExpr();
        std::ostringstream ss;
        expr->PrettyPrint(ss);
        ss << std::endl;
        LOG(INFO) << "[parse] " << ss.str();
    }
    catch (LexError e) {
        LOG(FATAL) << e.loc.row << ':' << e.loc.col << ' ' << e.msg;
    }
    catch (InternalCompilerError e) {
        LOG(FATAL) << e.msg;
    }

    return 0;
}
