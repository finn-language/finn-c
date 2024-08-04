#ifndef PARSER_HPP
#define PARSER_HPP

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "token.hpp"
#include "expr.hpp"
#include "errors.hpp"

#define TYPES TokenType::NUMBER, TokenType::INT8, TokenType::INT16, TokenType::INT32, TokenType::INT64, TokenType::INT128, \
              TokenType::UINT, TokenType::UINT8, TokenType::UINT16, TokenType::UINT32, TokenType::UINT64, TokenType::UINT128, \
              TokenType::FLOAT_TYPE, TokenType::DOUBLE_TYPE, TokenType::BOOL_TYPE, TokenType::STR, TokenType::INT_TYPE

class Parser {
    std::vector<std::shared_ptr<Token>> tokens;
    std::vector<std::string> lines;

    int index = 0;

    public:
        Parser(std::vector<std::shared_ptr<Token>> tokens, std::vector<std::string> lines) 
          : tokens(tokens), lines(lines) {
            this->index = index;
        }

        ~Parser() = default;

        std::vector<std::shared_ptr<Stmt::Stmt>> parse(void) {
            std::vector<std::shared_ptr<Stmt::Stmt>> statements = {};
            while (!this->at_end())
                statements.push_back(std::move(this->declaration()));
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
                Error* error = new Error(message, this->lines, this->previous());
                error->print();
                delete error;
                exit(1);
            }
        }

        std::shared_ptr<Stmt::Stmt> declaration(void) {
            switch (this->peek()->token_type) {

                case TokenType::TYPE: {
                    this->advance();
                    return this->type();
                }

                case TokenType::INTERFACE: {
                    this->advance();
                    return this->interface();
                }

                case TokenType::FUNC: {
                    this->advance();
                    return this->func();
                }

                case TokenType::STRUCT: {
                    this->advance();
                    return this->_struct();
                }

                case TokenType::ENUM: {
                    this->advance();
                    return this->_enum();
                }

                case TokenType::IMPORT: {
                    this->advance();
                    return this->import();
                }

                default: {
                    return this->control_flow();
                }

            }
        }

        std::shared_ptr<Stmt::Stmt> interface(void) {
            std::shared_ptr<Token> name = this->advance();
            this->consume(TokenType::L_BRACE, "Expected opening brace in interface definition");
            std::vector<std::shared_ptr<Stmt::Stmt>> body = {};

            while (!this->match({TokenType::R_BRACE}))
                body.push_back(this->declaration());

            return std::make_shared<Stmt::Interface>(Stmt::Interface(std::move(name), body));
        }

        std::shared_ptr<Stmt::Stmt> type(void) {
            std::shared_ptr<Stmt::Stmt> body;

            if (this->match({TokenType::STRUCT}))
                body = this->_struct();

            else if (this->match({TokenType::IDENT})) {

            }
        }

        std::shared_ptr<Stmt::Stmt> import(void) {
            std::shared_ptr<Expr::Expr> module = this->scope();
            this->consume(TokenType::SEMICOLON, "Expected semicolon at the end of statement");
            return std::make_shared<Stmt::Import>(Stmt::Import(std::move(module)));
        }

        std::shared_ptr<Stmt::Stmt> _enum(void) {
            std::shared_ptr<Token> enum_name = this->advance();
            std::vector<std::shared_ptr<Expr::Expr>> types = {};
            std::vector<std::shared_ptr<Stmt::Stmt>> body = {};

            if (this->match({TokenType::COLON})) {
                types.push_back(this->scope());

                while (this->match({TokenType::COMMA}))
                    types.push_back(this->scope());
            }

            this->consume(TokenType::L_BRACE, "Expected opening brace in enum definition");
            if (this->match({TokenType::IDENT})) {
                std::shared_ptr<Token> name = this->previous();
                std::shared_ptr<Expr::Expr> value = nullptr;

                if (this->match({TokenType::EQUAL}))
                    value = this->equality();

                body.push_back(std::make_shared<Stmt::Mutable>(Stmt::Mutable(std::move(name), types, std::move(value))));


                while (this->match({TokenType::COMMA})) {
                    std::shared_ptr<Token> name = this->advance();
                    std::shared_ptr<Expr::Expr> value = nullptr;

                    if (this->match({TokenType::EQUAL}))
                        value = this->equality();

                    body.push_back(std::make_shared<Stmt::Mutable>(Stmt::Mutable(std::move(name), types, std::move(value))));
                }
            }
            this->consume(TokenType::R_BRACE, "Expected closing brace in enum definition");


            return std::make_shared<Stmt::Enum>(Stmt::Enum(std::move(enum_name), types, std::move(body)));
        }

        std::shared_ptr<Stmt::Stmt> _struct(void) {
            std::shared_ptr<Token> name = this->advance();

            if (this->match({TokenType::LT})) {
                std::cout << "Struct generics are not implemented yet.";
                exit(1);
            }

            this->consume(TokenType::L_BRACE, "Expected opening brace in struct definition");

            std::vector<std::shared_ptr<Stmt::Stmt>> members = {};

            if (this->match({TokenType::IDENT})) {
                this->index--;
                members.push_back(this->arg());

                while (this->match({TokenType::COMMA})) 
                    members.push_back(this->arg());
                
            }

            this->consume(TokenType::R_BRACE, "Expected closing brace in struct definition");

            return std::make_shared<Stmt::Struct>(Stmt::Struct(std::move(name), members));
        }

        std::shared_ptr<Stmt::Stmt> func(void) {
            std::shared_ptr<Expr::Expr> name = this->literal();

            while (!this->match({TokenType::L_PAREN, TokenType::LT}) && this->match({TokenType::DOT})) {
                name = std::make_shared<Expr::Scope>(Expr::Scope(std::move(name), this->literal()));
            } this->index--;

            std::vector<std::shared_ptr<Stmt::Stmt>> args = {};

            std::vector<std::shared_ptr<Expr::Expr>> return_types = {};
            std::vector<std::shared_ptr<Expr::Expr>> throw_types = {};

            if (this->match({TokenType::LT})) {
                std::cout << "TODO: Function generics are not implemented yet\n";
                exit(1);
            }

            this->consume(TokenType::L_PAREN, "Expected opening parenthesis in function declaration");

            if (this->match({TokenType::IDENT})) {
                this->index--;
                args.push_back(std::move(this->arg()));
                while (this->match({TokenType::COMMA}))
                    args.push_back(std::move(this->arg()));
            }

            this->consume(TokenType::R_PAREN, "Expected closing parenthesis in function declaration");

            if (this->match({TokenType::COLON})) {

                if (this->match({TokenType::IDENT, TYPES})) {
                    this->index--;

                    return_types.push_back(this->scope());

                    while (this->match({TokenType::PIPE}))
                        return_types.push_back(this->scope());
                }                

                if (this->match({TokenType::QUESTION})) {
                    throw_types.push_back(this->scope());

                    while (this->match({TokenType::PIPE}))
                        throw_types.push_back(this->scope());
                }
            }

            this->consume(TokenType::L_BRACE, "guh");
            std::shared_ptr<Stmt::Stmt> body = this->block();

            return std::make_shared<Stmt::Func>(std::move(name), args, return_types, throw_types, std::move(body));
        }

        std::shared_ptr<Stmt::Stmt> arg(void) {
            std::shared_ptr<Token> name = this->advance();
            std::vector<std::shared_ptr<Expr::Expr>> types = {};
            std::shared_ptr<Expr::Expr> body = nullptr;

            this->consume(TokenType::COLON, "Expected colon in arg definition");

            types.push_back(this->prefix());
            while (this->match({TokenType::PIPE})) 
                types.push_back(this->prefix());
            
            if (this->match({TokenType::EQUAL}))
                body = this->equality();

            if (body == nullptr)
                return std::make_shared<Stmt::Mutable>(Stmt::Mutable(std::move(name), types, std::make_shared<Expr::Nil>(Expr::Nil())));
            else
                return std::make_shared<Stmt::Mutable>(Stmt::Mutable(std::move(name), types, std::move(body)));
        }

        std::shared_ptr<Stmt::Stmt> control_flow(void) {
            switch (this->peek()->token_type) {

                case TokenType::FOR: {
                    this->advance();
                    return std::move(this->for_loop());
                }

                case TokenType::WHILE: {
                    this->advance();
                    return std::move(this->while_loop());
                }

                case TokenType::IF: {
                    this->advance();
                    return std::move(this->if_else());
                }

                case TokenType::THROW: {
                    this->advance();
                    return this->_throw();
                }

                case TokenType::RETURN: {
                    this->advance();
                    return this->_return();
                }

                case TokenType::L_BRACE: {
                    this->advance();
                    return std::move(this->block());
                }

                default: {
                    return std::move(this->variables());
                }

            }
        }

        std::shared_ptr<Stmt::Stmt> _throw(void) {
            std::shared_ptr<Expr::Expr> body = this->equality();
            this->consume(TokenType::SEMICOLON, "Expected semicolon after throw statement");
            return std::make_shared<Stmt::Throw>(Stmt::Throw(std::move(body)));
        }

        std::shared_ptr<Stmt::Stmt> _return(void) {
            std::shared_ptr<Expr::Expr> body = this->equality();

            this->consume(TokenType::SEMICOLON, "Expected semicolon after return statement");

            return std::make_shared<Stmt::Return>(Stmt::Return(std::move(body)));
        }

        std::shared_ptr<Stmt::Stmt> for_loop(void) {
            bool is_finn_type_for_loop = false;
            bool is_c_type_for_loop = false;

            std::shared_ptr<Expr::Expr> name = nullptr;
            std::vector<std::shared_ptr<Expr::Expr>> types = {};
            std::shared_ptr<Expr::Expr> iterator = nullptr;

            std::shared_ptr<Stmt::Stmt> initial_variable = nullptr;
            std::shared_ptr<Expr::Expr> conditional = nullptr;
            std::shared_ptr<Expr::Expr> iterable = nullptr;

            std::shared_ptr<Stmt::Stmt> body = nullptr;

            if (this->match({TokenType::L_PAREN})) {
                if (this->match({TokenType::LET})) {
                    is_c_type_for_loop = true;
                    initial_variable = this->mutable_var();
                } else {
                    is_finn_type_for_loop = true;
                    name = this->literal();
                    this->consume(TokenType::COLON, "Expected colon in name definition");

                    types.push_back(this->literal());
                    while (this->match({TokenType::PIPE}))
                        types.push_back(this->literal());

                    this->consume(TokenType::R_PAREN, "Expected closing parenthesis in for loop");
                }
            }

            if (is_c_type_for_loop) {
                conditional = this->equality();
                this->consume(TokenType::SEMICOLON, "Expected semicolon after conditional");

                iterable = this->suffix();
                this->consume(TokenType::R_PAREN, "Expected closing parenthesis");
            }

            if (is_finn_type_for_loop) {
                this->consume(TokenType::COLON, "Expected colon in for loop");
                iterator = this->range();
            }


            if (this->match({TokenType::L_BRACE}))
                body = this->block();
            else
                body = this->control_flow();

            if (is_c_type_for_loop)
                return std::make_shared<Stmt::CFor>(Stmt::CFor(std::move(initial_variable), std::move(conditional), std::move(iterable), std::move(body)));
            else
                return std::make_shared<Stmt::FinnFor>(Stmt::FinnFor(std::move(name), types, std::move(iterator), std::move(body)));
        }

        std::shared_ptr<Stmt::Stmt> while_loop(void) {
            std::shared_ptr<Expr::Expr> conditional = this->equality();
            std::shared_ptr<Stmt::Stmt> body = this->control_flow();
            return std::make_shared<Stmt::While>(Stmt::While(std::move(conditional), std::move(body)));
        }

        std::shared_ptr<Stmt::Stmt> if_else(void) {
            //this->consume(TokenType::L_PAREN, "Expected opening parenthesis for conditional");
            std::shared_ptr<Expr::Expr> conditional = this->equality();
            //this->consume(TokenType::R_PAREN, "Expected closing parenthesis for conditional");

            std::shared_ptr<Stmt::Stmt> then_branch = this->control_flow();
            std::shared_ptr<Stmt::Stmt> else_branch = nullptr;

            if (this->match({TokenType::ELSE}))
                else_branch = this->control_flow();

            return std::make_shared<Stmt::If>(Stmt::If(std::move(conditional), std::move(then_branch), std::move(else_branch)));
        }

        std::shared_ptr<Stmt::Stmt> block(void) {
            std::vector<std::shared_ptr<Stmt::Stmt>> statements = {};

            while (!this->match({TokenType::R_BRACE}))
                statements.push_back(std::move(this->control_flow()));
            
            this->index--;
            this->consume(TokenType::R_BRACE, "Expected closing brace at the end of block");
            return std::make_shared<Stmt::Block>(Stmt::Block(statements));
        }

        std::shared_ptr<Stmt::Stmt> variables(void) {
            switch (this->peek()->token_type) {

                case TokenType::LET: {
                    this->advance();
                    return std::move(this->mutable_var());
                }

                case TokenType::CONST: {
                    this->advance();
                    return std::move(this->constant());
                }

                default: {
                    return std::move(this->expression());
                }

            }
        }

        std::shared_ptr<Stmt::Stmt> constant(void) {
            std::shared_ptr<Token> name = this->advance();
            std::vector<std::shared_ptr<Expr::Expr>> types = {};
            std::shared_ptr<Expr::Expr> value;

            if (this->match({TokenType::COLON})) {
                types.push_back(std::move(this->literal()));
                while (this->match({TokenType::PIPE})) {
                    types.push_back(std::move(this->literal()));
                }
            }

            if (this->match({TokenType::IDENT})) {
                std::cout << "Cannot have identifier before equals";
                exit(1);
            }

            this->consume(TokenType::EQUAL, "Expected equals operator in constant definition");
            value = std::move(this->equality());
            this->consume(TokenType::SEMICOLON, "Expected semicolon at the end of expression");
            return std::make_shared<Stmt::Constant>(Stmt::Constant(std::move(name), types, std::move(value)));
        }

        std::shared_ptr<Stmt::Stmt> mutable_var(void) {
            std::shared_ptr<Token> name = this->advance();
            std::vector<std::shared_ptr<Expr::Expr>> types = {};
            std::shared_ptr<Expr::Expr> value;

            bool is_static = false;

            if (this->match({TokenType::COLON})) {
                types.push_back(std::move(this->scope()));

                while (this->match({TokenType::PIPE})) 
                    types.push_back(std::move(this->scope()));
            }

            if (this->match({TokenType::IDENT})) {
                std::cout << "Cannot have identifier before equals";
                exit(1);
            }

            if (this->match({TokenType::SEMICOLON})) {
                value = std::make_shared<Expr::Nil>(Expr::Nil());
                return std::make_shared<Stmt::Mutable>(Stmt::Mutable(std::move(name), types, std::move(value)));
            }

            this->consume(TokenType::EQUAL, "Expected equals operator in variable definition");
            value = std::move(this->equality());
            this->consume(TokenType::SEMICOLON, "Expected semicolon at the end of expression");
            return std::make_shared<Stmt::Mutable>(Stmt::Mutable(std::move(name), types, std::move(value)));
        }

        std::shared_ptr<Stmt::Stmt> expression(void) {
            std::shared_ptr<Stmt::Stmt> expression = std::make_shared<Stmt::Expression>(Stmt::Expression(this->equality()));
            this->consume(TokenType::SEMICOLON, "Expected semicolon after expression");
            return std::move(expression);
        }

        std::shared_ptr<Expr::Expr> equality(void) {
            std::shared_ptr<Expr::Expr> expr = this->comparison();

            while (this->match({TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL})) {
                std::shared_ptr<Token> operand = this->previous();
                std::shared_ptr<Expr::Expr> right = this->comparison();
                expr = std::make_shared<Expr::Binary>(Expr::Binary(expr, operand, right));
            }

            return std::move(expr);
        }

        std::shared_ptr<Expr::Expr> comparison(void) {
            std::shared_ptr<Expr::Expr> expr = this->term();

            while (this->match({TokenType::GT, TokenType::GT_EQUALS, TokenType::LT, TokenType::LT_EQUALS})) {
                std::shared_ptr<Token> operand = this->previous();
                std::shared_ptr<Expr::Expr> right = this->term();
                expr = std::make_shared<Expr::Binary>(Expr::Binary(expr, operand, right));
            }

            return std::move(expr);
        }

        std::shared_ptr<Expr::Expr> term(void) {
            std::shared_ptr<Expr::Expr> expr = this->factor();

            while (this->match({TokenType::PLUS, TokenType::MINUS})) {
                std::shared_ptr<Token> operand = this->previous();
                std::shared_ptr<Expr::Expr> right = this->factor();
                expr = std::make_shared<Expr::Binary>(Expr::Binary(expr, operand, right));
            }

            return std::move(expr);
        }

        std::shared_ptr<Expr::Expr> factor(void) {
            std::shared_ptr<Expr::Expr> expr = this->range();

            while (this->match({TokenType::MULT, TokenType::DIV})) {
                std::shared_ptr<Token> operand = this->previous();
                std::shared_ptr<Expr::Expr> right = this->range();
                expr = std::make_shared<Expr::Binary>(Expr::Binary(expr, operand, right));
            }

            return std::move(expr);
        }

        std::shared_ptr<Expr::Expr> range(void) {
            std::shared_ptr<Expr::Expr> expr = this->suffix();

            while (this->match({TokenType::VARIADIC})) {
                std::shared_ptr<Token> operand = this->previous();
                std::shared_ptr<Expr::Expr> right = this->suffix();
                expr = std::make_shared<Expr::Binary>(Expr::Binary(expr, operand, right));
            }

            return std::move(expr);
        }

        std::shared_ptr<Expr::Expr> suffix(void) {
            std::shared_ptr<Expr::Expr> expr = this->prefix();

            while (this->match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS, TokenType::QUESTION, TokenType::BANG})) {
                std::shared_ptr<Token> operand = this->previous();
                expr = std::make_shared<Expr::Suffix>(Expr::Suffix(expr, operand));
            }

            return std::move(expr);
        }

        std::shared_ptr<Expr::Expr> prefix(void) {
            while (this->match({TokenType::MULT, TokenType::AMPERSAND, TokenType::MINUS, TokenType::VARIADIC, TokenType::PLUS_PLUS, TokenType::MINUS_MINUS})) {
                std::shared_ptr<Token> operand = this->previous();
                std::shared_ptr<Expr::Expr> expr = this->prefix();
                return std::move(std::make_shared<Expr::Prefix>(Expr::Prefix(expr, operand)));
            } return std::move(this->reassign());
        }

        std::shared_ptr<Expr::Expr> reassign(void) {
            std::shared_ptr<Expr::Expr> expr = this->scope();
            
            if (this->match({TokenType::EQUAL})) {
                expr = std::make_shared<Expr::Reassign>(Expr::Reassign(std::move(expr), this->equality()));
            }

            return expr;
        }

        std::shared_ptr<Expr::Expr> scope(void) {
            std::shared_ptr<Expr::Expr> expr = this->call();

            while (this->match({TokenType::DOT}))
                expr = std::make_shared<Expr::Scope>(Expr::Scope(std::move(expr), this->call()));
            
            return std::move(expr);
        }

        std::shared_ptr<Expr::Expr> call(void) {
            std::shared_ptr<Expr::Expr> expr = this->literal();

            if (this->match({TokenType::L_PAREN})) {
                std::vector<std::shared_ptr<Expr::Expr>> args = {};

                if (!this->match({TokenType::R_PAREN})) {
                    args.push_back(std::move(this->equality()));

                    while (this->match({TokenType::COMMA})) {
                        args.push_back(std::move(this->equality()));
                    }

                    this->consume(TokenType::R_PAREN, "Expected closing parenthesis");
                }

                expr = std::make_shared<Expr::Call>(Expr::Call(std::move(expr), args));
            }

            return std::move(expr);
        }

        std::shared_ptr<Expr::Expr> literal(void) {
            switch (this->peek()->token_type) {

                case TokenType::IDENT: {
                    this->advance();
                    return std::move(std::make_shared<Expr::Variable>(Expr::Variable(this->previous()->lexeme)));
                }

                case TokenType::STRING: {
                    this->advance();
                    return std::move(std::make_shared<Expr::StringLit>(Expr::StringLit(this->previous()->lexeme)));
                }

                case TokenType::NIL: {
                    this->advance();
                    return std::move(std::make_shared<Expr::Nil>(Expr::Nil()));
                }

                case TokenType::INT: {
                    this->advance();
                    return std::move(std::make_shared<Expr::IntLit>(Expr::IntLit(std::atoi(this->previous()->lexeme.c_str()))));
                }

                case TokenType::FLOAT: {
                    this->advance();
                    return std::make_shared<Expr::FloatLit>(Expr::FloatLit(std::atof(this->previous()->lexeme.c_str())));
                }

                case TokenType::TRUE: {
                    this->advance();
                    return std::move(std::make_shared<Expr::BoolLit>(Expr::BoolLit(true)));
                }

                case TokenType::FALSE: {
                    this->advance();
                    return std::move(std::make_shared<Expr::BoolLit>(Expr::BoolLit(false)));
                }

                case TokenType::L_PAREN: {
                    this->advance();
                    Expr::Grouping expression = Expr::Grouping(this->equality());
                    this->consume(TokenType::R_PAREN, "Expected closing parenthesis");
                    return std::move(std::make_shared<Expr::Grouping>(expression));
                }

                default: {

                    if (this->match({TYPES})) {
                        return std::make_shared<Expr::Type>(Expr::Type(this->previous()));
                    } else {
                        std::cout << this->peek()->lexeme << "\n";
                        Error("you messed up", lines, this->peek()).print();
                        exit(1);
                    }
                }

            }
        }
};

#endif
