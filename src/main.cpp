#include <iostream>
#include <fstream>
#include <memory>

#include "lib/lexer.hpp"
//#include "lib/parser.hpp"
#include "lib/expr.hpp"

std::string read_file(char* file_path) {
    std::string source = "";
    std::string line = "";

    std::ifstream source_file(file_path);

    if (source_file.is_open()) {
        while (std::getline(source_file, line)) {
            line.append("\n");
            source.append(line);
        }
    } else {
        std::cout << "Unable to open \"" << file_path << "\"\n";
        exit(1);
    }
    return source;
}

int main(int argc, char** argv) {
    std::string source = read_file(argv[1]);
    std::cout << "[INFO]: Successfully opened " << argv[1] << ".\n";

    std::string filename(argv[1]);

    std::vector<std::shared_ptr<Token>> tokens = (new Lexer(source, filename))->lex();
    std::cout << "[INFO]: Successfully lexed source.\n";

    int amount_of_tokens = 0;
    for (; amount_of_tokens <= tokens.size(); amount_of_tokens++) 
        std::cout << tokens[amount_of_tokens]->lexeme << "\n";

    std::cout << amount_of_tokens << "\n";


    std::cout << "guh\n";
    /*
    auto statements = (new Parser(tokens, filename))->parse();

    for (auto& statement : statements)
        statement->print();*/
    
    exit(0);
}