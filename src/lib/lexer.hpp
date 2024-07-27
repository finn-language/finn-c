#ifndef LEXER_HPP
#define LEXER_HPP

#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cctype>
#include <memory>

#include "token.hpp"

class Lexer {
    std::string source;
    std::string filename;

    int index;
    int start;

    int line;
    int cursor;

    std::vector<std::shared_ptr<Token>> output;

    public:
        inline Lexer(const std::string source, const std::string filename) {
            this->source = source;
            this->filename = filename;

            this->index = 0;
            this->start = 0;

            this->line = 1;
            this->cursor = 1;

            this->output = {};
        }

        inline ~Lexer() = default;

        inline std::vector<std::shared_ptr<Token>> lex(void) {
            while (!this->at_end()) {
                this->start = this->index;
                this->lex_token();
            }
            this->create_token(TokenType::END_OF_FILE, "", TokenType::INTRINSIC);
            return this->output;
        }

    private:
        inline bool at_end(void) {
            return this->index >= this->source.length();
        }

        inline bool match(char target) {
            if (this->at_end()) { return false; }
            if (target == this->source[this->index]) { this->advance(); return true; }
            return false;
        }

        inline char peek(void) { return this->source[this->index]; }

        inline char advance(void) {
            char character = this->source[this->index];
            this->index++;
            this->cursor++;
            return character;
        }

        inline void create_token(TokenType token_type, std::string lexeme, TokenType data_type) {
            Token token;
            token.token_type = token_type;
            token.lexeme = lexeme;
            token.data_type = data_type;
            token.position = {this->line, this->cursor};
            token.filename = this->filename;
            this->output.push_back(std::make_shared<Token>(token));
        }

        inline void string(void) {
            std::string buffer = "";

            while (!this->at_end() && this->peek() != '"') {
                if (this->peek() == '\n') {
                    this->line++;
                    this->cursor = 0;
                }
                buffer += this->peek();
                this->advance();
            }
            this->advance();
            this->create_token(TokenType::STRING, buffer, TokenType::STRING);
        }

        inline void number(void) {
            bool is_float = false;
            bool is_binary = false;
            bool is_hex = false;

            std::string buffer = "";
            buffer += this->source[this->index - 1];

            while (std::isdigit(this->peek())) {
                buffer += this->advance();
            }

            switch (this->peek()) {

                case 'b': {
                    is_binary = true;
                    buffer = "";
                    this->advance(); // move past 'b' character
                    while (this->peek() == '0' || this->peek() == '1')
                        buffer += this->advance();
                    break;
                }

                case 'x': {
                    is_hex = true;
                    buffer = "";
                    this->advance(); // move past 'x' character
                    while (std::isxdigit(this->peek()))
                        buffer += this->advance();
                    break;
                }

                case '.': {
                    this->advance();
                    if (this->peek() == '.') {
                        this->index--;
                        break;
                    }
                    is_float = true;
                    buffer += '.';
                    while (std::isdigit(this->peek()))
                        buffer += this->advance();
                    break;
                }

            }

            if (is_float) this->create_token(TokenType::FLOAT, buffer, TokenType::FLOAT);
            else if (is_hex) this->create_token(TokenType::INT, buffer, TokenType::INT);
            else if (is_binary) this->create_token(TokenType::INT, buffer, TokenType::INT);
            else this->create_token(TokenType::INT, buffer, TokenType::INT);
            
        }

        inline void identifier(void) {
            std::string buffer = "";
            buffer += this->source[this->index - 1];

            while (std::isalnum(this->peek()) || this->peek() == '_')
                buffer += this->advance();

            // the great if else wall (please keep it aligned, the wall must not fall)
            if      (buffer == "if")        this->create_token(TokenType::IF,        buffer, TokenType::INTRINSIC);
            else if (buffer == "else")      this->create_token(TokenType::ELSE,      buffer, TokenType::INTRINSIC);               
            else if (buffer == "for")       this->create_token(TokenType::FOR,       buffer, TokenType::INTRINSIC); 
            else if (buffer == "while")     this->create_token(TokenType::WHILE,     buffer, TokenType::INTRINSIC); 
            else if (buffer == "func")      this->create_token(TokenType::FUNC,      buffer, TokenType::INTRINSIC);
            else if (buffer == "struct")    this->create_token(TokenType::STRUCT,    buffer, TokenType::INTRINSIC);
            else if (buffer == "enum")      this->create_token(TokenType::ENUM,      buffer, TokenType::INTRINSIC);
            else if (buffer == "class")     this->create_token(TokenType::CLASS,     buffer, TokenType::INTRINSIC);
            else if (buffer == "final")     this->create_token(TokenType::FINAL,     buffer, TokenType::INTRINSIC);
            else if (buffer == "interface") this->create_token(TokenType::INTRINSIC, buffer, TokenType::INTRINSIC);
            else if (buffer == "return")    this->create_token(TokenType::RETURN,    buffer, TokenType::INTRINSIC);
            else if (buffer == "import")    this->create_token(TokenType::IMPORT,    buffer, TokenType::INTRINSIC);
            else if (buffer == "string")    this->create_token(TokenType::STR,       buffer, TokenType::INTRINSIC);
            else if (buffer == "int")       this->create_token(TokenType::INT,       buffer, TokenType::INTRINSIC);
            else if (buffer == "i8")        this->create_token(TokenType::INT8,      buffer, TokenType::INTRINSIC);
            else if (buffer == "i16")       this->create_token(TokenType::INT16,     buffer, TokenType::INTRINSIC);
            else if (buffer == "i32")       this->create_token(TokenType::INT32,     buffer, TokenType::INTRINSIC);
            else if (buffer == "i64")       this->create_token(TokenType::INT64,     buffer, TokenType::INTRINSIC);
            else if (buffer == "i128")      this->create_token(TokenType::INT128,    buffer, TokenType::INTRINSIC);
            else if (buffer == "u8")        this->create_token(TokenType::UINT8,     buffer, TokenType::INTRINSIC);
            else if (buffer == "u16")       this->create_token(TokenType::UINT16,    buffer, TokenType::INTRINSIC);
            else if (buffer == "u32")       this->create_token(TokenType::UINT32,    buffer, TokenType::INTRINSIC);
            else if (buffer == "u64")       this->create_token(TokenType::UINT64,    buffer, TokenType::INTRINSIC);
            else if (buffer == "u128")      this->create_token(TokenType::UINT128,   buffer, TokenType::INTRINSIC);
            else if (buffer == "bool")      this->create_token(TokenType::BOOL,      buffer, TokenType::INTRINSIC);
            else if (buffer == "float")     this->create_token(TokenType::FLOAT,     buffer, TokenType::INTRINSIC);
            else if (buffer == "double")    this->create_token(TokenType::DOUBLE,    buffer, TokenType::INTRINSIC);
            else if (buffer == "nil")       this->create_token(TokenType::NIL,       buffer, TokenType::INTRINSIC);
            else if (buffer == "true")      this->create_token(TokenType::TRUE,      buffer, TokenType::BOOL     );
            else if (buffer == "false")     this->create_token(TokenType::FALSE,     buffer, TokenType::BOOL     );
            else if (buffer == "type")      this->create_token(TokenType::TYPE,      buffer, TokenType::INTRINSIC);
            else if (buffer == "let")       this->create_token(TokenType::LET,       buffer, TokenType::INTRINSIC);
            else if (buffer == "const")     this->create_token(TokenType::CONST,     buffer, TokenType::INTRINSIC);
            else if (buffer == "static")    this->create_token(TokenType::STATIC,    buffer, TokenType::INTRINSIC);
            else                            this->create_token(TokenType::IDENT,     buffer, TokenType::IDENT    );
        }

        inline void lex_token(void) {
            char character = this->advance();

            switch (character) {

                case '.': {
                    if (this->match('.')) this->create_token(TokenType::VARIADIC, "..", TokenType::OPERAND);
                    else this->create_token(TokenType::DOT, ".", TokenType::OPERAND);
                    break;
                }

                case '+': {
                    if (this->match('+')) {
                        this->create_token(TokenType::PLUS_PLUS, "++", TokenType::OPERAND);
                    } else {
                        this->create_token(TokenType::PLUS, "+", TokenType::OPERAND);
                    } break;
                }

                case '-': {
                    if (this->match('-')) {
                        this->create_token(TokenType::MINUS_MINUS, "--", TokenType::OPERAND);
                        break;
                    } else {
                        this->create_token(TokenType::MINUS, "-", TokenType::OPERAND);
                        break;
                    }
                }

                case '@': {
                    this->create_token(TokenType::AT, "@", TokenType::OPERAND);
                    break;
                }

                case '/': {
                    if (this->match('/')) {
                        while (this->peek() != '\n')
                            this->advance();
                        break;
                    } else if (this->match('*')) {
                        bool is_looping = true;
                        while (is_looping) {
                            if (this->match('*')) {
                                if (this->match('/')) {
                                    is_looping = false;
                                    break;
                                }
                            } else {
                                this->advance();
                            }
                        } break;
                    } else {
                        this->create_token(TokenType::DIV, "/", TokenType::OPERAND);
                        break;
                    }
                }

                case '*': {
                    if (this->match('*')) {
                        this->create_token(TokenType::MULT_MULT, "**", TokenType::OPERAND);
                        break;
                    } else {
                        this->create_token(TokenType::MULT, "*", TokenType::OPERAND);
                        break;
                    }
                }

                case '<': {
                    if (this->match('<')) {
                        this->create_token(TokenType::SHL, "<<", TokenType::OPERAND);
                    } else if (this->match('=')) {
                        this->create_token(TokenType::GT_EQUALS, "<=", TokenType::OPERAND);
                    } else {
                        this->create_token(TokenType::GT, "<", TokenType::OPERAND);                    
                    } break;
                }

                case '>': {
                    if (this->match('>')) {
                        this->create_token(TokenType::SHR, ">>", TokenType::OPERAND);
                    } else if (this->match('=')) {
                        this->create_token(TokenType::LT_EQUALS, ">=", TokenType::OPERAND);
                    } else {
                        this->create_token(TokenType::LT, ">", TokenType::OPERAND);                    
                    } break;
                }

                case '=': {
                    if (this->match('=')) {
                        this->create_token(TokenType::EQUAL_EQUAL, "==", TokenType::OPERAND);
                        break;
                    } else {
                        this->create_token(TokenType::EQUAL, "=", TokenType::OPERAND);
                        break;
                    }
                }

                case '&': {
                    this->create_token(TokenType::AMPERSAND, "&", TokenType::OPERAND);
                    break;
                }

                case ';': {
                    this->create_token(TokenType::SEMICOLON, ";", TokenType::OPERAND);
                    break;
                }

                case ':': {
                    if (this->match('=')) {
                        this->create_token(TokenType::COLON_EQUAL, ":=", TokenType::OPERAND);
                        break;
                    } else if (this->match(':')) {
                        this->create_token(TokenType::SCOPE, "::", TokenType::OPERAND);
                        break;
                    } else {
                        this->create_token(TokenType::COLON, ":", TokenType::OPERAND);
                        break;
                    }
                }

                case '#': {
                    this->create_token(TokenType::HASH, "#", TokenType::OPERAND);
                    break;
                }

                case ',': {
                    this->create_token(TokenType::COMMA, ",", TokenType::OPERAND);
                    break;
                }

                case '|': {
                    this->create_token(TokenType::PIPE, "|", TokenType::OPERAND);
                    break;
                }

                case '!': {
                    this->create_token(TokenType::BANG, "!", TokenType::OPERAND);
                    break;
                }

                case '?': {
                    this->create_token(TokenType::QUESTION, "?", TokenType::OPERAND);
                    break;
                }

                case '(': {
                    this->create_token(TokenType::L_PAREN, "(", TokenType::OPERAND);
                    break;
                }

                case ')': {
                    this->create_token(TokenType::R_PAREN, ")", TokenType::OPERAND);
                    break;
                }

                case '{': {
                    this->create_token(TokenType::L_BRACE, "{", TokenType::OPERAND);
                    break;
                }

                case '}': {
                    this->create_token(TokenType::R_BRACE, "}", TokenType::OPERAND);
                    break;
                }

                case '[': {
                    this->create_token(TokenType::L_SQUARE, "[", TokenType::OPERAND);
                    break;
                }

                case ']': {
                    this->create_token(TokenType::R_SQUARE, "]", TokenType::OPERAND);
                    break;
                }

                case '"': {
                    this->string();
                    break;
                }

                case '\r': {
                    break;
                }
                case '\t': {
                    break;
                }
                case ' ': {
                    break;
                }

                case '\n': {
                    this->line++;
                    this->cursor = 1;
                    break;
                }

                default: {
                    if (std::isalpha(character) || character == '_') {
                        this->identifier();
                        break;
                    } else if (std::isdigit(character)) {
                        this->number();
                        break;
                    } else {
                        std::cout << this->filename << ":" << this->line << ":" << this->cursor << ": " 
                                  << "Found unexpected character \"" << this->source[this->index - 1] << "\"\n";
                        exit(1);
                    }
                }
            }
        }
};

#endif
