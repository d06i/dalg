#pragma once

#include <string>
#include <vector>
#include <iostream> 

enum Token : uint8_t {
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
    tok_ge,            // >=
    // bitwise Operators
    tok_or,            // || // wip
    tok_and,           // && // wip
    tok_not,           // && // wip
    tok_for,           //wip
    tok_while,         //wip  
    tok_comment_debug
};

struct TokenStore {
    std::string  name;
    Token        token_type;
    int          column = 0;
    int          line   = 0;
     
}; 

std::vector<TokenStore> lexer(const std::string& source);