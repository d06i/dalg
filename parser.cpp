#include "parser.h"

TokenStore& Parser::getCurrentToken() {

    if (currentToken >= tokens.size()) {
        static TokenStore eofTok = { "", tok_eof };
        return eofTok;
    }
    return tokens[currentToken];
}

TokenStore& Parser::getNextToken() {
    if (currentToken < tokens.size())
        currentToken++;
    return getCurrentToken();
}

std::unique_ptr<ExprAST> Parser::parseNumber() {
    auto res = std::make_unique<NumberExprAST>(std::stod(getCurrentToken().name));
    getNextToken();
    return res;
}

std::unique_ptr<ExprAST> Parser::parseString() {
    auto res = std::make_unique<StringExprAST>(getCurrentToken().name);
    getNextToken();
    return res;
}

std::unique_ptr<ExprAST> Parser::parsePrint() {
    getNextToken(); // skip "print"
    if (getCurrentToken().token_type != tok_left_paren)
        parserError("Expected '(' after 'print'");
    getNextToken(); // skip '('

    auto expr = parseExpression();
    if (!expr)
        std::cerr << "Invalid expression in print statement.\n";

    if (getCurrentToken().token_type != tok_right_paren)
        parserError("Expected ')' after print expression.");
    getNextToken(); // skip ')'

    return std::make_unique<PrintExprAST>(std::move(expr));
}

std::unique_ptr<ExprAST> Parser::parseIfElse() {
    getNextToken(); // skip 'if'

    auto cond = parseExpression();

    if (getCurrentToken().token_type != tok_left_brace)
        parserError("Expected '{' to start 'then' block.");
    getNextToken(); // skip '{'

    auto thenExpr = parseBlock();
    if (getCurrentToken().token_type != tok_right_brace)
        parserError("Expected '}' to end 'then' block.");
    getNextToken(); // skip '}'

    // Check for 'else'
    std::unique_ptr<ExprAST> elseExpr = nullptr;
    if (getCurrentToken().token_type == tok_else) {
        getNextToken(); // skip 'else'

        if (getCurrentToken().token_type != tok_left_brace)
            parserError("Expected '{' to start 'else' block.");
        getNextToken(); // skip '{'

        elseExpr = parseBlock();
        if (getCurrentToken().token_type != tok_right_brace)
            parserError("Expected '}' to end 'else' block.");
        getNextToken(); // skip '}'
    }

    if (!elseExpr)
        elseExpr = std::make_unique<BlockExprAST>(std::vector<std::unique_ptr<ExprAST>>());

    return std::make_unique<ifExprAST>(std::move(cond), std::move(thenExpr), std::move(elseExpr));
}

std::unique_ptr<ExprAST> Parser::parsePrimary() {
    switch (getCurrentToken().token_type) {
    case tok_identifier: return parseIdentifier();
    case tok_number:     return parseNumber();
    case tok_string:     return parseString();
    case tok_print:      return parsePrint();
    case tok_if:         return parseIfElse();
    default:
        parserError("Unknown token at position: " + std::to_string(currentToken) + " -> " + getCurrentToken().name);
    }
}

std::unique_ptr<ExprAST> Parser::parseFunctionCall(const std::string& callee) {
    getNextToken(); // skip '('
    std::vector<std::unique_ptr<ExprAST>> args;

    while (getCurrentToken().token_type != tok_right_paren) {
        args.push_back(parseExpression());

        if (getCurrentToken().token_type == tok_comma)
            getNextToken();
        else if (getCurrentToken().token_type != tok_right_paren)
            parserError("Expected ',' or ')' in function call.");
    }

    getNextToken(); // skip ')'

    return std::make_unique<CallExprAST>(callee, std::move(args));
}

std::unique_ptr<ExprAST> Parser::parseIdentifier() {
    std::string id = getCurrentToken().name;
    getNextToken();

    if (getCurrentToken().token_type == tok_left_paren)
        return parseFunctionCall(id);

    return std::make_unique<VariableExprAST>(id);
}

// a = 3 + 4 * 5  || b = 3 * 4 + 11
std::unique_ptr<ExprAST> Parser::parseBinaryOp( int min_prec ) {
    auto lhs = parsePrimary();
    if (!lhs) return nullptr;

    while (isOperator(getCurrentToken().token_type)) {
        std::string op = getCurrentToken().name;
        int precedence = op_precedence(op);

        if (precedence < min_prec)
            break;

        getNextToken(); // skip binOperator

        auto rhs = parseBinaryOp(precedence + 1); // recursive op_prec 
        if (!rhs) return nullptr;
        lhs = std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs));
    }

    return lhs;
}

std::unique_ptr<ExprAST> Parser::parseExpression() {
    if (getCurrentToken().token_type == tok_identifier &&
        tokens.size() > currentToken + 1 &&
        tokens[currentToken + 1].token_type == tok_equals)
        return parseAssignment();

    return parseBinaryOp(0);
}

std::unique_ptr<ExprAST> Parser::parseBlock() {
    std::vector<std::unique_ptr<ExprAST>> expr;

    while (getCurrentToken().token_type != tok_right_brace && getCurrentToken().token_type != tok_eof) {
        if (getCurrentToken().token_type == tok_semicolon) {
            getNextToken();
            continue;
        }

        if (getCurrentToken().token_type == tok_if) {
            expr.push_back(parseIfElse());
            continue;
        }

        auto temp = parseExpression();
        if (temp)
            expr.push_back(std::move(temp)); 
    }

    return std::make_unique<BlockExprAST>(std::move(expr));
}

// Assign Parsing -> "a = 3*b;"
std::unique_ptr<ExprAST> Parser::parseAssignment() {
    std::string n = getCurrentToken().name; // get identifier name
    getNextToken(); // skip identifier

    if (getCurrentToken().token_type != tok_equals) {
        parserError("Expected '=' after variable name.");
    }

    getNextToken(); // skip "="

    auto val = parseExpression();
    if (!val)
        return nullptr;

    if (getCurrentToken().token_type != tok_semicolon)
        parserError("Expected ';' after assignment.");
    getNextToken(); // skip ";" 

    return std::make_unique<AssignmentExprAST>(n, std::move(val));
}


std::unique_ptr<PrototypeAST> Parser::parsePrototype() {
    if (getCurrentToken().token_type != tok_identifier)
        parserError("Expected function name not available!");

    std::string FuncName = getCurrentToken().name;
    getNextToken(); // skip function name

    if (getCurrentToken().token_type != tok_left_paren)
      //  parserError( error_check(getCurrentToken()) + "Expected '(' after function name.") ;
    parserError("Expected '(' after function name.");
    getNextToken(); // skip '('

    std::vector<std::string> args;
    while (getCurrentToken().token_type != tok_right_paren) {
        if (getCurrentToken().token_type == tok_identifier)
            args.push_back(getCurrentToken().name);
        else
            parserError("Expected identifier in function arguments.");
        getNextToken();

        if (getCurrentToken().token_type == tok_comma)
            getNextToken();
        else if (getCurrentToken().token_type != tok_right_paren)
            parserError("Expected ',' or ')' in function arguments.");
    }

    getNextToken(); // skip ')'

    return std::make_unique<PrototypeAST>(FuncName, std::move(args));
}

std::unique_ptr<FunctionAST> Parser::parseFunction() {
    if (getCurrentToken().token_type != tok_fn)
        parserError("Expected 'fn' keyword not available! Current Token -> " + getCurrentToken().name);

    getNextToken(); // skip 'fn'

    if (getCurrentToken().token_type != tok_identifier)
        parserError("Expected function name not available!");

    auto proto = parsePrototype();

    if (getCurrentToken().token_type != tok_left_brace)
        parserError("Expected '{' to start function body.");
    getNextToken(); // skip '{'

    auto body = parseBlock();

    if (getCurrentToken().token_type != tok_right_brace)
        parserError("Expected '}' to end function body.");
    getNextToken(); // skip '}'

    return std::make_unique<FunctionAST>(std::move(proto), std::move(body));
}

bool Parser::isOperator(Token tok) {
    return tok == tok_plus || tok == tok_minus || tok == tok_multiply || tok == tok_divide ||
        tok == tok_eq || tok == tok_ne || tok == tok_lt || tok == tok_gt || tok == tok_le || tok == tok_ge;
}

void Parser::parserError(const std::string& msg) { 
    const auto token = getCurrentToken();
    const std::string currTokeInfo = " | Current token is => " + token.name;
    const std::string error = "Line: " + std::to_string(token.line) + " | " + msg + currTokeInfo; 
    throw std::runtime_error(error);
}
 
// https://en.cppreference.com/w/cpp/language/operator_precedence.html
int Parser::op_precedence( const std::string& op ) {
    std::map<std::string, int> op_pre;
    op_pre["*"] = 10;
    op_pre["/"] = 10;
    op_pre["%"] = 10;
    op_pre["+"] = 8;
    op_pre["-"] = 8;
    op_pre["<"] = 7;
    op_pre[">"] = 7;

    return op_pre[op] > 0 ? op_pre[op] : -1;
}