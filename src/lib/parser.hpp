#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include <memory>

#include "token.hpp"
#include "expr.hpp"

class Parser {
    std::vector<std::shared_ptr<Token>> tokens;
    std::string filename;

    int index = 0;

    public:
        Parser(std::vector<std::shared_ptr<Token>> tokens, std::string filename) {
            this->tokens = tokens;
            this->filename = filename;

            this->index = index;
        }

        ~Parser() = default;

        std::vector<std::shared_ptr<Stmt::Stmt>> parse(void) {
            std::vector<std::shared_ptr<Stmt::Stmt>> statements = {};
            while (!this->at_end())
                statements.push_back(std::make_shared<Stmt::Stmt>(this->expression()));
            return statements;
        }

    private:
        bool at_end(void) {
            return this->tokens[this->index]->token_type == TokenType::END_OF_FILE;
        }

        bool match(std::vector<TokenType> token_types) {
            for (TokenType &token_type : token_types) {
                if (token_type == this->peek()->token_type) {
                    this->advance();
                    return true;
                }
            } return false;
        }

        std::shared_ptr<Token> advance(void) {
            std::shared_ptr<Token> token = this->tokens[this->index];
            this->index++;
            return token;
        }

        std::shared_ptr<Token> peek(void) {
            return this->tokens[this->index];
        }

        std::shared_ptr<Token> previous(void) {
            return this->tokens[this->index - 1];
        }

        void consume(TokenType expected, std::string message) {
            if (!this->match({expected})) {
                std::cout << this->peek()->filename << ":" << this->peek()->position[0] << ":" << this->peek()->position[1] << ": " << message << "\n";
                exit(1);
            }
        }

        std::shared_ptr<Stmt::Stmt> expression(void) {
            std::shared_ptr<Stmt::Stmt> expression = std::make_shared<Stmt::Expression>(Stmt::Expression(this->equality()));
            this->consume(TokenType::SEMICOLON, "Expected semicolon after expression");
            return expression;
        }

        std::shared_ptr<Expr::Expr> equality(void) {
            std::shared_ptr<Expr::Expr> expr = this->comparison();

            while (this->match({TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL})) {
                std::shared_ptr<Token> operand = this->previous();
                std::shared_ptr<Expr::Expr> right = this->comparison();
                expr = std::make_shared<Expr::Expr>(Expr::Binary(expr, operand, right));
            }

            return expr;
        }

        std::shared_ptr<Expr::Expr> comparison(void) {
            std::shared_ptr<Expr::Expr> expr = this->term();

            while (this->match({TokenType::GT, TokenType::GT_EQUALS, TokenType::LT, TokenType::LT_EQUALS})) {
                std::shared_ptr<Token> operand = this->previous();
                std::shared_ptr<Expr::Expr> right = this->term();
                expr = std::make_shared<Expr::Binary>(Expr::Binary(expr, operand, right));
            }

            return expr;
        }

        std::shared_ptr<Expr::Expr> term(void) {
            std::shared_ptr<Expr::Expr> expr = this->factor();

            while (this->match({TokenType::PLUS, TokenType::MINUS})) {
                std::shared_ptr<Token> operand = this->previous();
                std::shared_ptr<Expr::Expr> right = this->factor();
                expr = std::make_shared<Expr::Binary>(Expr::Binary(expr, operand, right));
            }

            return expr;
        }

        std::shared_ptr<Expr::Expr> factor(void) {
            std::shared_ptr<Expr::Expr> expr = this->suffix();

            while (this->match({TokenType::MULT, TokenType::DIV})) {
                std::shared_ptr<Token> operand = this->previous();
                std::shared_ptr<Expr::Expr> right = this->suffix();
                expr = std::make_shared<Expr::Binary>(Expr::Binary(expr, operand, right));
            }

            return expr;
        }

        std::shared_ptr<Expr::Expr> suffix(void) {
            std::shared_ptr<Expr::Expr> expr = this->prefix();

            while (this->match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS, TokenType::QUESTION, TokenType::BANG})) {
                std::shared_ptr<Token> operand = this->previous();
                expr = std::make_shared<Expr::Suffix>(Expr::Suffix(expr, operand));
            }

            return expr;
        }

        std::shared_ptr<Expr::Expr> prefix(void) {
            while (this->match({TokenType::MULT, TokenType::AMPERSAND, TokenType::MINUS, TokenType::VARIADIC})) {
                std::shared_ptr<Token> operand = this->previous();
                std::shared_ptr<Expr::Expr> expr = this->prefix();
                return std::make_shared<Expr::Prefix>(Expr::Prefix(expr, operand));
            } return this->literal();
        }

        std::shared_ptr<Expr::Expr> literal(void) {
            switch (this->peek()->token_type) {

                case TokenType::STRING: {
                    return std::make_shared<Expr::StringLit>(Expr::StringLit(this->previous()->lexeme));
                }

                case TokenType::INT: {
                    this->advance();
                    return std::make_shared<Expr::IntLit>(Expr::IntLit(std::atoi(this->previous()->lexeme.c_str())));
                }

                case TokenType::L_PAREN: {
                    this->advance();
                    Expr::Expr expression = (Expr::Grouping(this->equality()));
                    this->consume(TokenType::R_PAREN, "Expected closing parenthesis");
                    return std::make_shared<Expr::Grouping>(expression);
                }

                default: {
                    std::cout << "you messed up\n";
                    exit(1);
                }

            }
        }
};

#endif