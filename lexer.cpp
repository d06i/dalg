#include "lexer.h"

namespace Lexer {
    std::vector<TokenStore> tokenz;
}

void lexer(const std::string& source) {
    size_t i = 0;
    while (i < source.length()) {
        char c = source[i];

        // skip whitespaces
        if (isspace(source[i])) {
            i++;
            continue;
        }

        // keyword check
        if (isalpha(c)) {
            std::string identifier;
            while (i < source.length() && (isalpha(source[i]) || source[i] == '_')) {
                identifier += source[i];
                i++;
            }

            if (identifier == "fn")
                Lexer::tokenz.push_back({ identifier, tok_fn });
            else if (identifier == "return")
                Lexer::tokenz.push_back({ identifier, tok_return });
            else if (identifier == "print")
                Lexer::tokenz.push_back({ identifier, tok_print });
            else if (identifier == "if")
                Lexer::tokenz.push_back({ identifier, tok_if });
            else if (identifier == "else")
                Lexer::tokenz.push_back({ identifier, tok_else });
            else
                Lexer::tokenz.push_back({ identifier, tok_identifier });
            continue;
        }

        // operator check
        if (c == '"') {

            std::string str;
            i++;

            while (i < source.length() && source[i] != '"') {
                str += source[i];
                i++;
            }

            if (i < source.length() && source[i] == '"') {
                Lexer::tokenz.push_back({ str, tok_string });
                i++;
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
            }
            Lexer::tokenz.push_back({ number, tok_number });
            continue;
        }

        // Compare Operators 
        if (c == '=') {
            if (i + 1 < source.length() && source[i + 1] == '=') {
                Lexer::tokenz.push_back({ "==", tok_eq });
                i += 2;
                continue;
            }
            else {
                Lexer::tokenz.push_back({ "=", tok_equals });
                i++;
                continue;
            }
        }

        if (c == '!') {
            if (i + 1 < source.length() && source[i + 1] == '=') {
                Lexer::tokenz.push_back({ "!=", tok_ne });
                i += 2;
                continue;
            }
            else {
                std::string unknown(1, source[i]);
                Lexer::tokenz.push_back({ unknown, tok_unk });
                i++;
                continue;
            }
        }

        if (c == '<') {
            if (i + 1 < source.length() && source[i + 1] == '=') {
                Lexer::tokenz.push_back({ "<=", tok_le });
                i += 2;
                continue;
            }
            else {
                Lexer::tokenz.push_back({ "<", tok_lt });
                i++;
                continue;
            }
        }

        if (c == '>') {
            if (i + 1 < source.length() && source[i + 1] == '=') {
                Lexer::tokenz.push_back({ ">=", tok_ge });
                i += 2;
                continue;
            }
            else {
                Lexer::tokenz.push_back({ ">", tok_gt });
                i++;
                continue;
            }
        }

        // Special characters
        switch (c) {
        case '{':
            Lexer::tokenz.push_back({ "{", tok_left_brace });
            i++;
            continue;
        case '}':
            Lexer::tokenz.push_back({ "}", tok_right_brace });
            i++;
            continue;
        case '(':
            Lexer::tokenz.push_back({ "(", tok_left_paren });
            i++;
            continue;
        case ')':
            Lexer::tokenz.push_back({ ")", tok_right_paren });
            i++;
            continue;
        case '+':
            Lexer::tokenz.push_back({ "+", tok_plus });
            i++;
            continue;
        case '-':
            Lexer::tokenz.push_back({ "-", tok_minus });
            i++;
            continue;
        case '*':
            Lexer::tokenz.push_back({ "*", tok_multiply });
            i++;
            continue;
        case '/':
            Lexer::tokenz.push_back({ "/", tok_divide });
            i++;
            continue;
        case ';':
            Lexer::tokenz.push_back({ ";", tok_semicolon });
            i++;
            continue;
        case ',':
            Lexer::tokenz.push_back({ ",", tok_comma });
            i++;
            continue;
        }

        // unknowns
        std::string unknown(1, source[i]);
        Lexer::tokenz.push_back({ unknown, tok_unk });
        i++;
    }
}