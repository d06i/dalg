#pragma once

#include <vector>
#include <memory>
#include <map>

#include "lexer.h"
#include "ast.h"

class Parser {
    std::vector<TokenStore>& tokens;
    size_t currentToken = 0; 
public:
    Parser(std::vector<TokenStore>& t) : tokens(t) {}

    TokenStore& getCurrentToken();
    TokenStore& getNextToken();
    std::unique_ptr<ExprAST> parseNumber();
    std::unique_ptr<ExprAST> parseString();
    std::unique_ptr<ExprAST> parsePrint();
    std::unique_ptr<ExprAST> parseIfElse();
    std::unique_ptr<ExprAST> parsePrimary();
    std::unique_ptr<ExprAST> parseFunctionCall(const std::string& callee);
    std::unique_ptr<ExprAST> parseIdentifier();
    std::unique_ptr<ExprAST> parseBinaryOp(int min_prec);
    std::unique_ptr<ExprAST> parseExpression();
    std::unique_ptr<ExprAST> parseBlock();
    std::unique_ptr<ExprAST> parseAssignment();
    std::unique_ptr<PrototypeAST> parsePrototype();
    std::unique_ptr<FunctionAST> parseFunction();

    bool isOperator(Token tok); 
    void parserError(const std::string& msg);
    int  op_precedence(const std::string& op);
};  