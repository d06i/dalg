#include <llvm/Transforms/Scalar.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/IR/PassManager.h>

#include "parser.h"

// LLVM Optimizations
void optimize() {
    llvm::PassBuilder passBuilder;

    llvm::LoopAnalysisManager lam;
    llvm::FunctionAnalysisManager fam;
    llvm::CGSCCAnalysisManager cgam;
    llvm::ModuleAnalysisManager mam;

    passBuilder.registerModuleAnalyses(mam);
    passBuilder.registerCGSCCAnalyses(cgam);
    passBuilder.registerFunctionAnalyses(fam);
    passBuilder.registerLoopAnalyses(lam);
    passBuilder.crossRegisterProxies(lam, fam, cgam, mam);

    llvm::ModulePassManager mpm = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3);

    mpm.run(*Module, mam);
}

// Token Write
void write() {
    for (auto& i : Lexer::tokenz) {
        std::cout << i.name << " -> ";
        switch (i.token_type) {
        case tok_identifier: std::cout << "identifier"; break;
        case tok_fn: std::cout << "function"; break;
        case tok_return: std::cout << "return"; break;
        case tok_number: std::cout << "number"; break;
        case tok_left_brace: std::cout << "left_brace"; break;
        case tok_right_brace: std::cout << "right_brace"; break;
        case tok_left_paren: std::cout << "left_paren"; break;
        case tok_right_paren: std::cout << "right_paren"; break;
        case tok_eof: std::cout << "EOF"; break;
        case tok_unk: std::cout << "Unknown"; break;
        case tok_equals: std::cout << "Assign"; break;
        case tok_semicolon: std::cout << "Semicolon"; break;
        case tok_string: std::cout << "String"; break;
        case tok_if: std::cout << "if"; break;
        case tok_else: std::cout << "else"; break;
        case tok_eq: std::cout << "=="; break;
        case tok_ne: std::cout << "!="; break;
        case tok_lt: std::cout << "<"; break;
        case tok_gt: std::cout << ">"; break;
        case tok_le: std::cout << "<="; break;
        case tok_ge: std::cout << ">="; break;
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}