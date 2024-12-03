#include "parser.h"
#include "utility.h"

void initializeLLVM() {

    Module = std::make_unique<llvm::Module>("TEST", Context);
    if (!Module)
        std::cout << "[initializeLLVM] Module is failed!";
}


void compile_Run( const std::string& filename ) {
    initializeLLVM();

    auto code = readFile(filename);
    lexer(code);

    Parser parser(Lexer::tokenz);

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

} 

void usage() {

    std::cout << "\n****** LLVM based dalg language by d06i ***********\n" <<
        "For LLVM IR code : dalg.exe input.dlag output.ll \n" <<
        "For executable file: clang output.ll -o output.exe\n";

}
 
int main( int argc, const char* argv[] ) {

    if (argc < 3) {
        usage();
        return -1;
    }

    if (argc == 3) {
        compile_Run( argv[1] );
        write2File ( argv[2] );
        std::cout << "LLVM IR writed!\n";
    }
    else
        std::cerr << "Write failed!\n";
     

    return 0;
}
