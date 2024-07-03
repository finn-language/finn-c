#ifndef LEXER_HPP
#define LEXER_HPP

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
                    while (std::isdigit(this->peek()))
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
                    is_float = true;
                    buffer += '.';
                    this->advance(); // move past '.' character
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
        

            std::string text = buffer;

            // the great if else wall (please keep it aligned, the wall must not fall)
            if      (text == "if")     this->create_token(TokenType::IF,      text, TokenType::INTRINSIC);
            else if (text == "else")   this->create_token(TokenType::ELSE,    text, TokenType::INTRINSIC);               
            else if (text == "for")    this->create_token(TokenType::FOR,     text, TokenType::INTRINSIC); 
            else if (text == "while")  this->create_token(TokenType::WHILE,   text, TokenType::INTRINSIC); 
            else if (text == "func")   this->create_token(TokenType::FUNC,    text, TokenType::INTRINSIC);
            else if (text == "struct") this->create_token(TokenType::STRUCT,  text, TokenType::INTRINSIC);
            else if (text == "enum")   this->create_token(TokenType::ENUM,    text, TokenType::INTRINSIC);
            else if (text == "impl")   this->create_token(TokenType::IMPL,    text, TokenType::INTRINSIC);
            else if (text == "return") this->create_token(TokenType::RETURN,  text, TokenType::INTRINSIC);
            else if (text == "import") this->create_token(TokenType::IMPORT,  text, TokenType::INTRINSIC);
            else if (text == "int")    this->create_token(TokenType::INT,     text, TokenType::INTRINSIC);
            else if (text == "i8")     this->create_token(TokenType::INT8,    text, TokenType::INTRINSIC);
            else if (text == "i16")    this->create_token(TokenType::INT16,   text, TokenType::INTRINSIC);
            else if (text == "i32")    this->create_token(TokenType::INT32,   text, TokenType::INTRINSIC);
            else if (text == "i64")    this->create_token(TokenType::INT64,   text, TokenType::INTRINSIC);
            else if (text == "i128")   this->create_token(TokenType::INT128,  text, TokenType::INTRINSIC);
            else if (text == "u8")     this->create_token(TokenType::UINT8,   text, TokenType::INTRINSIC);
            else if (text == "u16")    this->create_token(TokenType::UINT16,  text, TokenType::INTRINSIC);
            else if (text == "u32")    this->create_token(TokenType::UINT32,  text, TokenType::INTRINSIC);
            else if (text == "u64")    this->create_token(TokenType::UINT64,  text, TokenType::INTRINSIC);
            else if (text == "u128")   this->create_token(TokenType::UINT128, text, TokenType::INTRINSIC);
            else if (text == "bool")   this->create_token(TokenType::BOOL,    text, TokenType::INTRINSIC);
            else if (text == "float")  this->create_token(TokenType::FLOAT,   text, TokenType::INTRINSIC);
            else if (text == "double") this->create_token(TokenType::DOUBLE,  text, TokenType::INTRINSIC);
            else if (text == "nil")    this->create_token(TokenType::NIL,     text, TokenType::INTRINSIC);
            else if (text == "true")   this->create_token(TokenType::TRUE,    text, TokenType::BOOL     );
            else if (text == "false")  this->create_token(TokenType::FALSE,   text, TokenType::BOOL     );
            else if (text == "type")   this->create_token(TokenType::TYPE,    text, TokenType::INTRINSIC);
            else                       this->create_token(TokenType::IDENT,   text, TokenType::IDENT    );
            
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

                case '/': {
                    if (this->match('/')) {
                        while (this->peek() != '\n')
                            this->advance();
                        break;
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