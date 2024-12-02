
#include "parser.h"
#include "utility.h"

void initializeLLVM() {

    Module = std::make_unique<llvm::Module>("TEST", Context);
    if (!Module)
        std::cout << "[initializeLLVM] Module is failed!";
}


void compile_Run(std::vector<TokenStore>& tokens) {
    initializeLLVM();
    Parser parser(tokens);

    while (parser.getCurrentToken().token_type != tok_eof) {
        auto func = parser.parseFunction();
        if (!func)
            throw std::runtime_error("Function parsing failed!");

        func->codegen();

    }

    std::string verifyOutput;
    llvm::raw_string_ostream rso(verifyOutput);
    if (llvm::verifyModule(*Module, &rso))
        std::cerr << "[MODULE] ->" << verifyOutput << "\n";

    optimize();
    Module->print(llvm::outs(), nullptr);
}



int main() {

    const std::string& code = R"(
        fn q(a, b) { a + b }
        
        fn string_test() { 
            c = "blah blah blah!!!"; 
            d = 4 + 1; 
            print(d)
        }
     
    )";

    lexer(code);

    write();

    try
    {
        std::cout << code << "\n\n";
        compile_Run(Lexer::tokenz);
    }
    catch (const std::exception& e)
    {
        std::cout << "[!] " << e.what();
    }

    return 0;
}
