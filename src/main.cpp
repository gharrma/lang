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
                LOG(FATAL) << BuildStr("Could not open file \'", argv[i], '\'');
            Lexer lexer(file);
            Parser parser(lexer);
            TypeChecker type_checker;
            std::vector<std::unique_ptr<Node>> asts;
            try {
                llvm::LLVMContext context;
                llvm::Module mod("Main", context);
                Emitter emitter(mod);
                while (lexer.Peek()) {
                    type_checker.errors.clear();
                    asts.push_back(parser.ParseTopLevelConstruct());
                    auto& ast = **asts.rbegin();
                    std::cout << ast << std::endl;
                    ast.Accept(type_checker);
                    for (const auto& e : type_checker.errors)
                        FILE_ERROR(typecheck);
                    if (!type_checker.errors.empty())
                        continue;
                    ast.Accept(emitter);
                }
                verifyModule(mod, &llvm::errs());
                std::cout << std::endl;
                mod.print(llvm::outs(), nullptr);
                std::cout << std::endl;
            }
            catch (const LexError& e)   { FILE_ERROR(lex); }
            catch (const ParseError& e) { FILE_ERROR(parse); }
        }
    }
    else {
        // REPL.
        Lexer lexer(std::cin);
        Parser parser(lexer);
        TypeChecker type_checker;
        std::vector<std::unique_ptr<Node>> asts;
        llvm::LLVMContext context;
        llvm::Module mod("REPL", context);
        Emitter emitter(mod);
        while (true) {
            try {
                std::cout << "c> ";
                asts.push_back(parser.ParseTopLevelConstruct());
                auto& ast = **asts.rbegin();
                lexer.Get(kSemicolon);
                std::cout << "\n[parse]\n";
                std::cout << ast << std::endl;
                std::cout << '\n';
                type_checker.errors.clear();
                ast.Accept(type_checker);
                for (const auto& e : type_checker.errors)
                    REPL_ERROR(typecheck);
                if (!type_checker.errors.empty())
                    continue;
                std::cout << "[ir] " << std::flush;
                ast.Accept(emitter);
                if (auto expr = dynamic_cast<Expr*>(&ast)) {
                    auto val = emitter.vals.at(expr);
                    val->print(llvm::outs());
                    std::cout << std::endl;
                } else {
                    mod.rbegin()->print(llvm::outs());
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
