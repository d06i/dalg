#pragma once

#include <map>
#include <iostream>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

// Globals
extern llvm::LLVMContext Context;
extern llvm::IRBuilder<> Builder;
extern std::unique_ptr<llvm::Module> Module;
extern std::map<std::string, llvm::Value*> NamedValues;

// All Expressions
class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual llvm::Value* codegen() = 0;
};

// Numbers
class NumberExprAST : public ExprAST {
    double val;
public:
    NumberExprAST(double x) : val(x) {}

    llvm::Value* codegen();
};

// Strings
class StringExprAST : public ExprAST {
    std::string str;
public:
    StringExprAST(const std::string& s) : str(s) {}

    llvm::Value* codegen();

};

// Variables
class VariableExprAST : public ExprAST {
    std::string name;
public:
    VariableExprAST(std::string& x) : name(x) {}

    llvm::Value* codegen();
};

// Binary Operands
class BinaryExprAST : public ExprAST {
    std::string op;
    std::unique_ptr<ExprAST> lhs, rhs;
public:
    BinaryExprAST(const std::string& x, std::unique_ptr<ExprAST> l, std::unique_ptr<ExprAST> r)
        : op(x), lhs(std::move(l)), rhs(std::move(r)) {
    }

    llvm::Value* codegen();
};

// Func prototype -> fn test(a,b)
class PrototypeAST : public ExprAST {
    std::string name;
    std::vector<std::string> Args;
public:
    PrototypeAST(std::string x, std::vector<std::string> a) : name(x), Args(a) {}

    const std::string& getName() const {
        return name;
    }

    llvm::Function* codegen();
};

// Function Call
class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;
public:
    CallExprAST(std::string c, std::vector<std::unique_ptr<ExprAST>> x) : Callee(c), Args(std::move(x)) {}

    llvm::Value* codegen();
};

// Function
class FunctionAST : public ExprAST {
    std::unique_ptr<PrototypeAST> proto;
    std::unique_ptr<ExprAST> body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> x, std::unique_ptr<ExprAST> y)
        : proto(std::move(x)), body(std::move(y)) {
    }

    llvm::Function* codegen();
};

// Assigment
class AssignmentExprAST : public ExprAST {
    std::string name;
    std::unique_ptr<ExprAST> val;
public:
    AssignmentExprAST(const std::string& x, std::unique_ptr<ExprAST> y)
        : name(x), val(std::move(y)) {
    }

    llvm::Value* codegen();
};


// Block Expression
class BlockExprAST : public ExprAST {
    std::vector<std::unique_ptr<ExprAST>> expr;
public:
    BlockExprAST(std::vector<std::unique_ptr<ExprAST>> x) : expr(std::move(x)) {}

    llvm::Value* codegen();
};

// Printf linking  
class PrintExprAST : public ExprAST {
    std::unique_ptr<ExprAST> expr;
public:
    PrintExprAST(std::unique_ptr<ExprAST> x) : expr(std::move(x)) {}

    llvm::Value* codegen();
};

// If-Else Expresion
class ifExprAST : public ExprAST {
    std::unique_ptr<ExprAST> Cond, Then, Else;
public:
    ifExprAST(std::unique_ptr<ExprAST> cond, std::unique_ptr<ExprAST> thenExpr, std::unique_ptr<ExprAST> elseExpr)
        : Cond(std::move(cond)), Then(std::move(thenExpr)), Else(std::move(elseExpr)) {
    }

    llvm::Value* codegen();
};