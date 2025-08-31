#include "ast.h"

llvm::LLVMContext Context;
llvm::IRBuilder<> Builder(Context);
std::unique_ptr<llvm::Module> g_Module;
std::map<std::string, llvm::Value*> NamedValues;
 
// Numbers
llvm::Value* NumberExprAST::codegen() {
	return llvm::ConstantFP::get(Context, llvm::APFloat(val));
}

// Strings
llvm::Value* StringExprAST::codegen() {
	if (str.empty())
		throw std::runtime_error("String is empty");

	return Builder.CreateGlobalStringPtr(str, "string");
}

// Variables
llvm::Value* VariableExprAST::codegen() {
	llvm::Value* V = NamedValues[name];
	if (!V)
		throw std::runtime_error("[VariableExprAST] Unknown variable name: " + name);

	if (V->getType()->isPointerTy())
		return Builder.CreateLoad(llvm::Type::getDoubleTy(Context), V, name);
	else
		return V;
}

// Binary Operands
llvm::Value* BinaryExprAST::codegen() {
	llvm::Value* L = lhs->codegen();
	llvm::Value* R = rhs->codegen();

	if (!L || !R)
		throw std::runtime_error("[BinaryExprAST] LHS or RHS create is failed!");

	if (op == "+")
		return Builder.CreateFAdd(L, R, "addtmp");
	if (op == "-")
		return Builder.CreateFSub(L, R, "subtmp");
	if (op == "*")
		return Builder.CreateFMul(L, R, "multmp");
	if (op == "/")
		return Builder.CreateFDiv(L, R, "divtmp");

	if (op == "==")
		return Builder.CreateFCmpOEQ(L, R, "equal");
	if (op == "!=")
		return Builder.CreateFCmpONE(L, R, "notEqual");
	if (op == "<")
		return Builder.CreateFCmpOLT(L, R, "less");
	if (op == ">")
		return Builder.CreateFCmpOGT(L, R, "greater");
	if (op == "<=")
		return Builder.CreateFCmpOLE(L, R, "lessOrEqual");
	if (op == ">=")
		return Builder.CreateFCmpOGE(L, R, "greaterOrEqual");

	throw std::runtime_error("[BinaryExprAST] Invalid binary operator: " + op);
}

// Func prototype -> fn test(a,b)
llvm::Function* PrototypeAST::codegen() {
	std::vector<llvm::Type*> doubles(Args.size(), llvm::Type::getDoubleTy(Context));
	llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(Context), doubles, false);
	llvm::Function* F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, g_Module.get());

	uint64_t idx = 0;
	for (auto& a : F->args())
		a.setName(Args[idx++]);

	return F;
}

// Function Call
llvm::Value* CallExprAST::codegen() {
	llvm::Function* CalleeFunc = g_Module->getFunction(Callee);
	if (!CalleeFunc)
		throw std::runtime_error("[CallExprAST] Unknown function referenced: " + Callee);

	if (CalleeFunc->arg_size() != Args.size())
		throw std::runtime_error("[CallExprAST] Incorrect number of arguments passed to function: " + Callee);

	std::vector<llvm::Value*> ArgsV;
	for (size_t i = 0, e = Args.size(); i != e; i++) {
		ArgsV.push_back(Args[i]->codegen());
		if (!ArgsV.back())
			return nullptr;
	}

	return Builder.CreateCall(CalleeFunc, ArgsV, "calltmp");
}

// Functions
llvm::Function* FunctionAST::codegen() {
	llvm::Function* func = proto->codegen();
	if (!func)
		return nullptr;

	llvm::BasicBlock* bb = llvm::BasicBlock::Create(Context, "entry", func);
	Builder.SetInsertPoint(bb);

	NamedValues.clear();

	for (auto& arg : func->args()) {
		llvm::AllocaInst* alloca = Builder.CreateAlloca(arg.getType(), nullptr, arg.getName());
		Builder.CreateStore(&arg, alloca);
		NamedValues[std::string(arg.getName())] = alloca;
	}

	if (llvm::Value* retVal = body->codegen()) {
		Builder.CreateRet(retVal);
		llvm::verifyFunction(*func);
		return func;
	}

	func->eraseFromParent();
	return nullptr;
}

// Assigment
llvm::Value* AssignmentExprAST::codegen() {
	llvm::Value* value = val->codegen();
	if (!value)
		std::cerr << "[AssignmentExprAST] RHS not created.\n";

	llvm::Value* var = NamedValues[name];
	if (!var) {
		llvm::AllocaInst* alloca = Builder.CreateAlloca(llvm::Type::getDoubleTy(Context), nullptr, name);
		Builder.CreateStore(value, alloca);
		NamedValues[name] = alloca;
	}
	else
		Builder.CreateStore(value, var);

	return value;
}

// Block Expression
llvm::Value* BlockExprAST::codegen() {
	llvm::Value* last = nullptr;
	for (auto& i : expr) {
		last = i->codegen();
		if (!last)
			return nullptr;
	}
	return last;
}

// Printf linking 
llvm::Value* PrintExprAST::codegen() {
	llvm::Value* val = expr->codegen();
	if (!val)
		std::cerr << "Expression failed.\n";

	llvm::Function* PrintfFunc = g_Module->getFunction("printf");
	if (!PrintfFunc) {
		llvm::FunctionType* printfType = llvm::FunctionType::get(
			llvm::Type::getInt32Ty(Context),
			llvm::Type::getInt8PtrTy(Context),
			true
		);

		PrintfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", g_Module.get());
	}

	llvm::Value* formatSTR = nullptr;
	if (val->getType()->isPointerTy())
		formatSTR = Builder.CreateGlobalStringPtr("%s\n", "str");
	else if (val->getType()->isDoubleTy())
		formatSTR = Builder.CreateGlobalStringPtr("%f\n", "str");
	else
		std::cerr << "Unsupported type for printf";

	Builder.CreateCall(PrintfFunc, { formatSTR, val }, "printfCall");

	return llvm::ConstantFP::get(Context, llvm::APFloat(0.0));
}

// If-Else Expresion
llvm::Value* ifExprAST::codegen() {
	llvm::Value* condV = Cond->codegen();
	if (!condV)
		throw std::runtime_error("[IfExprAST] Condition expression failed.");

	// Convert condition to a boolean by comparing non-equal to 0.0
	if (condV->getType()->isDoubleTy()) {
		// Convert floating-point to boolean by comparing to 0.0
		condV = Builder.CreateFCmpONE(condV, llvm::ConstantFP::get(Context, llvm::APFloat(0.0)), "ifcond");
	}
	else if (condV->getType()->isIntegerTy(1)) {
		// Already a boolean, no need to convert
	}
	else
		throw std::runtime_error("[IfExprAST] Unsupported condition type.");


	llvm::Function* function = Builder.GetInsertBlock()->getParent();

	llvm::BasicBlock* thenBlock  = llvm::BasicBlock::Create(Context, "then", function);
	llvm::BasicBlock* elseBlock  = llvm::BasicBlock::Create(Context, "else");
	llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(Context, "merge");

	Builder.CreateCondBr(condV, thenBlock, elseBlock);

	// Then block
	Builder.SetInsertPoint(thenBlock);
	llvm::Value* thenVar = Then->codegen();
	if (!thenVar)
		throw std::runtime_error("[IfExprAST] Then expression failed.");
	Builder.CreateBr(mergeBlock);
	thenBlock = Builder.GetInsertBlock();

	// Else block
	function->getBasicBlockList().push_back(elseBlock);
	Builder.SetInsertPoint(elseBlock);

	llvm::Value* elseVar = nullptr;
	if (Else) {
		elseVar = Else->codegen();
		if (!elseVar)
			elseVar = llvm::ConstantFP::get(Context, llvm::APFloat(0.0));
	}
	else
		elseVar = llvm::ConstantFP::get(Context, llvm::APFloat(0.0));


	Builder.CreateBr(mergeBlock);
	elseBlock = Builder.GetInsertBlock();
	function->getBasicBlockList().push_back(mergeBlock);

	Builder.SetInsertPoint(mergeBlock);

	// Merge block
	llvm::PHINode* phi = Builder.CreatePHI(llvm::Type::getDoubleTy(Context), 2, "if_tmp");
	phi->addIncoming(thenVar, thenBlock);
	phi->addIncoming(elseVar, elseBlock);

	return phi;
}

// for expression -> for x=0, x < 10, 1 { Body }  
llvm::Value* forExprAST::codegen(){

	llvm::Value* start = Start->codegen();
	if (!start)
		return nullptr;

	BasicBlock* tempBlock = Builder.GetInsertBlock();
	llvm::Function* func =  Builder.GetInsertBlock()->getParent();
	
	llvm::BasicBlock* startBlock = BasicBlock::Create(Context, "start", func);  
	
	Builder.CreateBr(startBlock);
	Builder.SetInsertPoint(startBlock);

	PHINode* var_phi = Builder.CreatePHI(Type::getDoubleTy(Context), 2, VarName);
	var_phi->addIncoming(start, tempBlock);

	llvm::AllocaInst* alloca = Builder.CreateAlloca(Type::getDoubleTy(Context), nullptr, VarName);
	Builder.CreateStore(var_phi, alloca);
	Value* oldVal = NamedValues[VarName];
	NamedValues[VarName] = alloca;

	if (!Body->codegen())
		return nullptr;

	// step
	Value* stepVal = nullptr;
	if (Step) {
		stepVal = Step->codegen();
		if (!stepVal)
			return nullptr;
	}
	else
		stepVal = ConstantFP::get(Context, APFloat(1.0));

	// next iter
	Value* nextVar = Builder.CreateFAdd(var_phi, stepVal, "nextVar");

	// end
	Value* EndCond = End->codegen();
	if (!EndCond)
		return nullptr;

	Value* tempCond =nullptr;
	if (EndCond->getType()->isDoubleTy())
		tempCond = Builder.CreateFCmpONE(EndCond, ConstantFP::get(Context, APFloat(0.0)), "loopcond");
	else
		tempCond = EndCond;

	BasicBlock* loopEnd = Builder.GetInsertBlock();
	BasicBlock* AfterBlock = BasicBlock::Create(Context, "afterLoop", func);

	var_phi->addIncoming(nextVar, loopEnd);

	Builder.CreateCondBr(tempCond, startBlock, AfterBlock);
	Builder.SetInsertPoint(AfterBlock);
	 
	if (oldVal)
		NamedValues[VarName] = oldVal; // update val
	else
		NamedValues.erase(VarName);

	return Constant::getNullValue(Type::getDoubleTy(Context));
}