#include "parser.h"
#include "utility.h"

void initializeLLVM() {

	Module = std::make_unique<llvm::Module>("TEST", Context);
	if (!Module)
		std::cout << "[initializeLLVM] Module is failed!";
}


void compile_Run(const std::string& filename) {
	initializeLLVM();

	const auto code = readFile(filename);
	auto tokens = lexer(code);

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

}

void usage() {

	std::cout << "\n****** LLVM based dalg language by d06i ***********\n" <<
		"For LLVM IR code : dalg.exe input.dlag output.ll \n" <<
		"For executable file: clang output.ll -o output.exe\n";

}

int main(int argc, const char* argv[]) {

	try {

		if (argc == 2) {
			const auto src = readFile(argv[1]);
			auto token = lexer(src);
			write(token);
		}

		if (argc == 3) {
			std::cout << "Compiling...\n";
			compile_Run(argv[1]);
			write2File(argv[2]);
			std::cout << "LLVM IR writed!\n";
		}
		else
			std::cerr << "Write failed!\n";
	}
	catch (const std::exception& err) {
		std::cerr << "Error: " << err.what() << "\n";
		usage();
		return 1;
	}

	return 0;
}
