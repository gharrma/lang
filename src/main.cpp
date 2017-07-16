#include <iostream>
#include <fstream>
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
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
        try {
            while (lexer.Peek()) {
                auto token = lexer.Get();
                std::cout << token;
                if (lexer.Peek()) std::cout << ' ';
            }
            std::cout << std::endl;
        }
        catch (const LexError& e) {
            REPL_ERROR(lex);
            exit(1);
        }
    }
    else if (argc > 1) {
        // Parse from files.
        int i = 1;
        for (; i < argc; ++i) {
            std::ifstream file(argv[i]);
            if (!file)
                LOG(FATAL) << BuildStr("Could not open file ", argv[i]);
            Lexer lexer(file);
            Parser parser(lexer);
            try {
                // TODO: Make sure to verify module once we do codegen here.
                while (lexer.Peek()) {
                    auto ast = parser.ParseTopLevelConstruct();
                    std::cout << *ast << std::endl;
                    auto type_errors = TypeCheck(*ast);
                    for (const auto& e : type_errors)
                        FILE_ERROR(typecheck);
                    if (!type_errors.empty()) {
                        continue;
                    }
                }
            }
            catch (const LexError& e)   { FILE_ERROR(lex); }
            catch (const ParseError& e) { FILE_ERROR(parse); }
        }
    }
    else {
        // REPL.
        Lexer lexer(std::cin);
        Parser parser(lexer);
        while (true) {
            llvm::LLVMContext context;
            llvm::Module mod("REPL", context);
            try {
                std::cout << "c> ";
                auto ast = parser.ParseTopLevelConstruct();
                lexer.Get(kSemicolon);
                std::cout << "\n[parse]\n";
                std::cout << *ast << std::endl;
                std::cout << '\n';
                auto type_errors = TypeCheck(*ast);
                for (const auto& e : type_errors)
                    REPL_ERROR(typecheck);
                if (!type_errors.empty())
                    continue;
                std::cout << "[ir] " << std::flush;
                if (auto expr = dynamic_cast<Expr*>(ast.get())) {
                    auto val = EmitExpr(*expr, mod);
                    val->dump();
                    std::cout << std::endl;
                } else {
                    Emit(*ast, mod);
                    mod.rbegin()->dump();
                }
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
