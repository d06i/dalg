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
    ExprPtr parseNumber();
    ExprPtr parseString();
    ExprPtr parsePrint();
    ExprPtr parseIfElse();
    ExprPtr parsePrimary();
    ExprPtr parseFunctionCall(const std::string& callee);
    ExprPtr parseIdentifier();
    ExprPtr parseBinaryOp(int min_prec);
    ExprPtr parseExpression();
    ExprPtr parseBlock();
    ExprPtr parseAssignment();
    ExprPtr parseElse();
    ExprPtr parseFor();
 //   ExprPtr parseWhile();
    std::unique_ptr<PrototypeAST> parsePrototype();
    std::unique_ptr<FunctionAST> parseFunction();

    bool isOperator(Token tok); 
    void parserError(const std::string& msg);
    int  op_precedence(const std::string& op);
};  