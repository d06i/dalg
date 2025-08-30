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

class ExprAST;
using ExprPtr = std::unique_ptr<ExprAST>;

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
    ExprPtr lhs, rhs;
public:
    BinaryExprAST(const std::string& x, ExprPtr l, ExprPtr r)
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
    std::vector<ExprPtr> Args;
public:
    CallExprAST(std::string c, std::vector<ExprPtr> x) : Callee(c), Args(std::move(x)) {}

    llvm::Value* codegen();
};

// Function
class FunctionAST : public ExprAST {
    std::unique_ptr<PrototypeAST> proto;
    ExprPtr body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> x, ExprPtr y)
        : proto(std::move(x)), body(std::move(y)) {
    }

    llvm::Function* codegen();
};

// Assigment
class AssignmentExprAST : public ExprAST {
    std::string name;
    ExprPtr val;
public:
    AssignmentExprAST(const std::string& x, ExprPtr y)
        : name(x), val(std::move(y)) {
    }

    llvm::Value* codegen();
};
 
// Block Expression
class BlockExprAST : public ExprAST {
    std::vector<ExprPtr> expr;
public:
    BlockExprAST(std::vector<ExprPtr> block_vec ) : expr(std::move( block_vec )) {}

    bool isEmpty() const {
        return expr.empty();
    }

    llvm::Value* codegen();
};

// Printf linking  
class PrintExprAST : public ExprAST {
    ExprPtr expr;
public:
    PrintExprAST(ExprPtr x) : expr(std::move(x)) {}

    llvm::Value* codegen();
};

// If-Else Expresion
class ifExprAST : public ExprAST {
    ExprPtr Cond, Then, Else;
public:
    ifExprAST(ExprPtr cond, ExprPtr thenExpr, ExprPtr elseExpr)
        : Cond(std::move(cond)), Then(std::move(thenExpr)), Else(std::move(elseExpr)) {
    }

    llvm::Value* codegen();
};