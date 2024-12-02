#pragma once

#include <string>
#include <vector>
#include <iostream>

enum Token {
    tok_fn,
    tok_return,
    tok_number,
    tok_identifier,
    tok_equals,        // =
    tok_left_brace,    // {
    tok_right_brace,   // }
    tok_left_paren,    // (
    tok_right_paren,   // )
    tok_plus,          // +
    tok_minus,         // -
    tok_multiply,      // *
    tok_divide,        // /
    tok_semicolon,     // ;
    tok_string,
    tok_comma,         // , 
    tok_print,
    tok_if,
    tok_else,
    tok_eof,
    tok_unk,
    // Compare Operators
    tok_eq,            // ==
    tok_ne,            // !=
    tok_lt,            // <
    tok_gt,            // >
    tok_le,            // <=
    tok_ge             // >=
};

struct TokenStore {
    std::string name;
    Token token_type;
};

namespace Lexer {
    extern std::vector<TokenStore> tokenz;
}

void lexer(const std::string& source);