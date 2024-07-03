#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <vector>

//#include "..llvm/IRReader/IRReader.h"

typedef enum {
    STRING = 0,

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

    FLOAT,
    DOUBLE,

    BOOL,

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

    OR, // or
    AND, // and
    NOT, // not

    TRUE,
    FALSE,

    AMPERSAND, // &

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
    IMPL,

    RETURN,

    END_OF_FILE, // for some reason g++ panics when you have "EOF" as a value
} TokenType;

struct Token {
    TokenType token_type;
    std::string lexeme;
    TokenType data_type;
    std::vector<int> position;
    std::string filename;
};

#endif