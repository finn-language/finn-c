#ifndef ERRORS_HPP
#define ERRORS_HPP

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cctype>
#include <iostream>

#include "token.hpp"

class Error {
    std::string              message;
    std::shared_ptr<Token>   token;
    std::vector<std::string> lines;

    std::string output = "";

    public:
        Error(std::string message, std::vector<std::string> lines, std::shared_ptr<Token> token)
          : message(message), lines(lines), token(std::move(token)) {}

        ~Error() = default;

        void print(void) {
            std::cout << this->build_error();

        }

    private:
        std::string add_whitespace(std::string output, int indent) {
            for (int i = 1; i <= indent; i++)
                output += " ";
            return output;
        }

        std::string build_error(void) {
            std::string output = "";
            std::cout << this->token->position.offset[0] << " " << this->token->position.offset[1] << "\n";
            output += this->token->filename + ":" + std::to_string(this->token->position.line) + ":" + std::to_string(this->token->position.offset[1]) + ":\n";
            std::cout << this->lines[this->token->position.line];
            output += " " + std::to_string(this->token->position.line) + " | " + this->lines[this->token->position.line - 1];

            output = this->add_whitespace(output, std::to_string(this->token->position.line).length() + 2); 
            output += "|";
            output = this->add_whitespace(output, this->token->position.offset[0]);

            for (int i = this->token->position.offset[0]; i == this->token->position.offset[1]; i++)
                output += "^";
            output += "\n";

            output = this->add_whitespace(output, std::to_string(this->token->position.line).length() + 2); 
            this->output += "|";
            output = this->add_whitespace(output, std::to_string(this->token->position.offset[0]).length());

            output += this->message + "\n";
            return output;
        }
};

#endif