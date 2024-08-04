#ifndef TOKEN_HPP
#define TOKEN_HPP

#pragma once

#include <string>
#include <vector>

typedef enum {
    STRING = 0,
    NUMBER,
    BOOL,
    FLOAT,
    DOUBLE,
    UINT,

    INT_TYPE,

    INT,
    INT8,
    INT16,
    INT32,
    INT64,
    INT128,

    UINT8,
    UINT16,
    UINT32,
    UINT64,
    UINT128,

    FLOAT_TYPE,
    DOUBLE_TYPE,

    BOOL_TYPE,

    STR,

    NIL,

    OPERAND,
    INTRINSIC,
    IDENT,

    GT, // >
    GT_EQUALS, // >=
    SHL, // >>

    LT, // <
    LT_EQUALS, // <=
    TYPE_EQUALITY, // <=>
    SHR, // <<

    PLUS, // +
    PLUS_PLUS, // ++

    MINUS, // -
    MINUS_MINUS, // --

    DIV, // /
    
    MULT, // *
    MULT_MULT, // **

    EQUAL, // =
    EQUAL_EQUAL, // ==
    FAT_ARROW, // =>

    OR, // or
    AND, // and
    NOT, // not

    TRUE,
    FALSE,

    AMPERSAND, // &
    AT, // @

    LET,
    CONST,

    STATIC,

    COMMA, // ,
    PIPE, // |

    HASH, // #
    QUESTION, // ?

    BANG, // !
    NOT_EQUAL, // !=

    SEMICOLON, // ;

    COLON, // :
    COLON_EQUAL, // :=
    SCOPE, // ::

    DOT, // .
    VARIADIC, // ..

    L_PAREN, // (
    R_PAREN, // )

    L_BRACE, // {
    R_BRACE, // }

    L_SQUARE, // [
    R_SQUARE, // ]

    IMPORT,

    IMPLY,
    TYPE,

    IF, 
    ELSE,

    FOR,
    WHILE,

    FUNC,
    ENUM,
    STRUCT,

    FINAL,
    CLASS,
    
    INTERFACE,

    RETURN,
    THROW,

    END_OF_FILE // for some reason g++ panics when you have "EOF" as a value
} TokenType;

struct Position {
    int line;
    std::vector<int> offset;
};

struct Token {
    TokenType        token_type;
    std::string      lexeme;
    TokenType        data_type;
    Position         position;
    std::string      filename;

    std::string dump(int indent = 0) {
        std::string root = ""; 
        
        for (int i = 1; i <= indent; i++)
            root += " "; root += "Token(" + this->lexeme + ")\n";

        return root;
    }
};

#endif
