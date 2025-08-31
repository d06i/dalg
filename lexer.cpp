#include "lexer.h"
  
std::vector<TokenStore> lexer(const std::string& source) {

    std::vector<TokenStore> tokenz;
    size_t i = 0;
    int line = 1, column = 1;

    while (i < source.length()) {
        char c = source[i];

        // skip whitespaces
        if (isspace(source[i])) {
            if (source[i] == '\n') {
                line++;
                column = 1;
            } else
                column++;
            i++; 
            continue;
        }   
 
        // comment
        if ( c == '#') { 
            i++; 
            column++;
            while (i < source.length() && source[i] != '\n') {
                i++;
                column++;
            } 
            continue;
        }
                 
        // keywords
        if (isalpha(c)) {
            std::string identifier;
            while (i < source.length() && (isalpha(source[i]) || source[i] == '_')) {
                identifier += source[i];
                i++;
                column++;
            }

            // search keywords in hash table 
            auto keyw = keywords.find(identifier);
            if (keyw == keywords.end())
                tokenz.push_back({ identifier, tok_identifier, column, line });
            else
                tokenz.push_back({ identifier, keyw->second,   column, line }); 

            continue;
        }

        // Strings
        if (c == '"') {

            std::string str;
            i++;
            column++;

            while (i < source.length() && source[i] != '"') {
                str += source[i];
                i++;
                column++;
            }

            if (i < source.length() && source[i] == '"') {
                tokenz.push_back({ str, tok_string });
                i++;
                column++;
            }
            else
                std::cerr << "String literal not closed.";

            continue;
        }

        // Numbers
        if (isdigit(c)) {
            std::string number;
            while (i < source.length() && (isdigit(source[i]) || source[i] == '.')) {
                number += source[i];
                i++;
                column++;
            }   
            tokenz.push_back({ number, tok_number, column, line });
            continue;
        }

        // Compare Operators 
        if (c == '=') {
            if (i + 1 < source.length() && source[i + 1] == '=') {
                tokenz.push_back({ "==", tok_eq, column, line });
                i += 2;
                column += 2;
                continue;
            }
            else {
                tokenz.push_back({ "=", tok_equals, column, line });
                i++;
                column++;
                continue;
            }
        }

        if (c == '!') {
            if (i + 1 < source.length() && source[i + 1] == '=') {
                tokenz.push_back({ "!=", tok_ne, column, line });
                i += 2;
                column += 2;
                continue;
            }
            else {
                std::string unknown(1, source[i]);
                tokenz.push_back({ unknown, tok_unk, column, line });
                i++;
                column++;
                continue;
            }
        }

        if (c == '<') {
            if (i + 1 < source.length() && source[i + 1] == '=') {
                tokenz.push_back({ "<=", tok_le, column, line });
                i += 2;
                column += 2;
                continue;
            }
            else {
                tokenz.push_back({ "<", tok_lt, column, line });
                i++;
                column++;
                continue;
            }
        }

        if (c == '>') {
            if (i + 1 < source.length() && source[i + 1] == '=') {
                tokenz.push_back({ ">=", tok_ge, column, line });
                i += 2;
                column += 2;
                continue;
            }
            else {
                tokenz.push_back({ ">", tok_gt, column, line });
                i++;
                column++;
                continue;
            }
        }

        // Special characters
        switch (c) {
        case '{':
            tokenz.push_back({ "{", tok_left_brace, column, line });
            i++;
            column++;
            continue;
        case '}':
            tokenz.push_back({ "}", tok_right_brace, column, line });
            i++;
            column++;
            continue;
        case '(':
            tokenz.push_back({ "(", tok_left_paren, column, line });
            i++;
            column++;
            continue;
        case ')':
            tokenz.push_back({ ")", tok_right_paren, column, line });
            i++;
            column++;
            continue;
        case '+':
            tokenz.push_back({ "+", tok_plus, column, line });
            i++;
            column++;
            continue;
        case '-':
            tokenz.push_back({ "-", tok_minus, column, line });
            i++;
            column++;
            continue;
        case '*':
            tokenz.push_back({ "*", tok_multiply, column, line });
            i++;
            column++;
            continue;
        case '/':
            tokenz.push_back({ "/", tok_divide, column, line });
            i++;
            column++;
            continue;
        case ';':
            tokenz.push_back({ ";", tok_semicolon, column, line });
            i++;
            column++;
            continue;
        case ',':
            tokenz.push_back({ ",", tok_comma, column, line });
            i++;
            column++;
            continue;
        }

        // unknowns
        std::string unknown(1, source[i]);
        tokenz.push_back({ unknown, tok_unk, column, line });
        i++;
        column++;
    }

    return tokenz;
} 