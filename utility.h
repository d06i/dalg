#include <llvm/Transforms/Scalar.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/IR/PassManager.h>

#include "parser.h"

#include <sstream>
#include <fstream>

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
void write(const std::vector<TokenStore>& tokens) {
    for (const auto& i : tokens) {
        std::cout << i.name << " -> ";
        switch (i.token_type) {
        case tok_identifier:    std::cout << "identifier"; break;
        case tok_fn:            std::cout << "function"; break;
        case tok_return:        std::cout << "return"; break;
        case tok_number:        std::cout << "number"; break;
        case tok_left_brace:    std::cout << "left_brace"; break;
        case tok_right_brace:   std::cout << "right_brace"; break;
        case tok_left_paren:    std::cout << "left_paren"; break;
        case tok_right_paren:   std::cout << "right_paren"; break;
        case tok_eof:           std::cout << "EOF"; break;
        case tok_unk:           std::cout << "Unknown"; break;
        case tok_equals:        std::cout << "Assign"; break;
        case tok_semicolon:     std::cout << "Semicolon"; break;
        case tok_string:        std::cout << "String"; break;
        case tok_if:            std::cout << "if"; break;
        case tok_else:          std::cout << "else"; break;
        case tok_eq:            std::cout << "=="; break;
        case tok_ne:            std::cout << "!="; break;
        case tok_lt:            std::cout << "<"; break;
        case tok_gt:            std::cout << ">"; break;
        case tok_le:            std::cout << "<="; break;
        case tok_ge:            std::cout << ">="; break;
        case tok_comma:         std::cout << "comma"; break;
        case tok_comment_debug: std::cout << "comment"; break;
        case tok_plus:          std::cout << "+"; break;
        case tok_minus:         std::cout << "-"; break;
        case tok_multiply:      std::cout << "*"; break;
        case tok_divide:        std::cout << "/"; break;
        case tok_print:         std::cout << "print"; break;
        case tok_or:            std::cout << "||"; break;
        case tok_and:           std::cout << "&&"; break;
        case tok_not:           std::cout << "!"; break;
        case tok_for:           std::cout << "for"; break;
        case tok_while:         std::cout << "while"; break;
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}


std::string readFile(const std::string& filename) {

    std::ifstream file(filename);
    std::string temp;
    std::ostringstream oss;

    if (!file){
        std::cerr << filename << " is not found!\n";
        return "";
    }

    while (std::getline(file, temp))
        oss << temp << "\n";

    return oss.str();
}

void write2File(const std::string& filename, bool opt = false) {

    std::error_code error;
    llvm::raw_fd_ostream filestream(filename , error);

    if (error) {
        llvm::errs() << "File not found! -> " << error.message() << "\n";
        return;
    }

    if (opt)
        optimize();

    Module->print(filestream, nullptr);

    filestream.close();
}
