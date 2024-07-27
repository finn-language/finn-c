#include <iostream>
#include <fstream>
#include <memory>

#include "lib/lexer.hpp"
#include "lib/parser.hpp"
#include "lib/expr.hpp"

bool exists(char* name) {
    return static_cast<bool>(std::ifstream(name));
}

std::string read_file(char* file_path) {
    std::string source = "";
    std::string line = "";

    std::ifstream source_file(file_path);

    if (source_file.is_open()) {
        while (std::getline(source_file, line)) {
            source.append(line + "\n");
        }
    } else {
        std::cout << "Unable to open \"" << file_path << "\"\n";
        exit(1);
    }
    return source;
}

int main(int argc, char** argv) {
    std::string filename = "";
    char* c_filename     = nullptr;
    bool time_comp       = false;
    bool be_quiet        = false;
    bool show_token      = false;
    bool show_ast        = false;

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--timecomp") {
            time_comp = true;
        }

        else if (std::string(argv[i]) == "--token") {
            show_token = true;
        }
        
        else if (std::string(argv[i]) == "--quiet") {
            be_quiet = true;
        }

        else if (std::string(argv[i]) == "--ast") {
            show_ast = true;
        }
        
        else if (exists(argv[i])) {
            filename = argv[i];
            c_filename = argv[i];
        }
    }
    
    if (filename == "") {
        std::cout << "No valid file was provided";
        return 1;
    }
        
    std::string source = read_file(c_filename);

    if (!be_quiet) 
        std::cout << "[INFO]: Successfully opened " << filename << ".\n";

    Lexer* lexer = new Lexer(source, filename);
    std::vector<std::shared_ptr<Token>> tokens = lexer->lex();
    delete lexer;
    
    if (!be_quiet)
        std::cout << "[INFO]: Successfully lexed source.\n";

    if (show_token) {
        int amount_of_tokens = 0;
        for (; amount_of_tokens <= tokens.size(); amount_of_tokens++) 
            std::cout << tokens[amount_of_tokens]->lexeme << "\n";

        std::cout << amount_of_tokens << "\n";
    }

    Parser* parser = new Parser(tokens, filename);
    std::vector<std::shared_ptr<Stmt::Stmt>> statements = parser->parse();
    delete parser;
    
    if (!be_quiet)
        std::cout << "[INFO]: Successfully parsed source.\n";

    if (show_ast) {
        for (const std::shared_ptr<Stmt::Stmt> statement : statements) {
            statement->print();
            std::cout << "\n";
        }
        std::cout << statements.size() << "\n";
    }

    return 0;
}
