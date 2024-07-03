#ifndef EXPR_HPP
#define EXPR_HPP

#include <iostream>
#include <memory>

#include "token.hpp"
#include "compiler.hpp"

/*
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
*/

namespace Expr {

class Expr {
    public:
        virtual ~Expr() = default;

        void print(int indent);
        //virtual llvm::Value* codegen() = 0;
};

class Binary : public Expr {
    std::shared_ptr<Expr>  left;
    std::shared_ptr<Token> operand;
    std::shared_ptr<Expr>  right;

    public:
        Binary(std::shared_ptr<Expr> left, std::shared_ptr<Token> operand, std::shared_ptr<Expr> right) : left(std::move(left)), operand(std::move(operand)), right(std::move(right)) {}

        void print(int indent = 0) {
            for (int i = 0; i <= indent; i++)
                std::cout << " ";
            std::cout << "Expr.Binary(\n";
            this->left->print(indent + 4);
            std::cout << "Token(" << this->operand->lexeme << ")\n";
            this->right->print(indent + 4);
            for (int i = 0; i <= indent; i++)
                std::cout << " ";
            std::cout << ")\n";
        }

        //llvm::Value* codegen(std::shared_ptr<llvm::LLVMContext> context) {
            //return llvm::Constant
        //}
};

class Suffix : public Expr {
    std::shared_ptr<Expr>  left;
    std::shared_ptr<Token> operand;

    public:
        Suffix(std::shared_ptr<Expr> left, std::shared_ptr<Token> operand) : left(std::move(left)), operand(std::move(operand)) {}

        void print(int indent = 0) {
            for (int i = 0; i <= indent; i++)
                std::cout << " ";
            std::cout << "Expr.Suffix(\n";
            this->left->print(indent + 4);
            std::cout << "Token(" << this->operand->lexeme << ")\n";
            for (int i = 0; i <= indent; i++)
                std::cout << " ";
            std::cout << ")\n";
        }
};

class Prefix : public Expr {
    std::shared_ptr<Expr>  right;
    std::shared_ptr<Token> operand;

    public:
        Prefix(std::shared_ptr<Expr> right, std::shared_ptr<Token> operand) : right(std::move(right)), operand(std::move(operand)) {}

        void print(int indent = 0) {
            for (int i = 0; i <= indent; i++)
                std::cout << " ";
            std::cout << "Expr.Suffix(\n";
            this->right->print(indent + 4);
            std::cout << "Token(" << this->operand->lexeme << ")\n";
            for (int i = 0; i <= indent; i++)
                std::cout << " ";
            std::cout << ")\n";
        }
};

class Grouping : public Expr {
    std::shared_ptr<Expr> expression;

    public:
        Grouping(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

        void print(int indent = 0) {
            for (int i = 0; i <= indent; i++)
                std::cout << " ";
            std::cout << "Expr.Grouping(\n";
            this->expression->print(indent + 4);
            for (int i = 0; i <= indent; i++)
                std::cout << " ";
            std::cout << ")\n";
        }
};

class IntLit : public Expr {
    int value;

    public:
        IntLit(int value) : value(value) {}

        void print(int indent = 0) {
            for (int i = 0; i <= indent; i++)
                std::cout << " ";
            std::cout << "Expr.Literal(" << this->value << ")\n";
        }
        //llvm::Value* codegen(std::shared_ptr<llvm::LLVMContext> context) {
        //    return llvm::ConstantInt(context, llvm::Type::getInt32Ty(*context.get()), llvm::APInt(*this->value.get()));
        //}
};

class StringLit : public Expr {
    std::string value;

    public:
        StringLit(std::string value) : value(value) {}

        void print(int indent = 0) {
            for (int i = 0; i <= indent; i++)
                std::cout << " ";
            std::cout << "Expr.Literal(" << this->value << ")\n";
        }
};
}


namespace Stmt {
class Stmt {
    public:
        virtual ~Stmt() = 0;
        virtual void print(int indent = 0) = 0;
};

class Expression : public Stmt {
    std::shared_ptr<Expr::Expr> expression;

    public:
        Expression(std::shared_ptr<Expr::Expr> expression) : expression(std::move(expression)) {}

        void print(int indent = 0) {
            for (int i = 0; i <= indent; i++)
                std::cout << " ";
            std::cout << "Stmt.Expression(\n";
            this->expression->print(indent + 4);
            for (int i = 0; i <= indent; i++)
                std::cout << " ";
            std::cout << ")\n";
        }
};

}
#endif