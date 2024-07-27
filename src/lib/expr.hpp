#ifndef EXPR_HPP
#define EXPR_HPP

#include <iostream>
#include <memory>

#pragma once

#include <llvm/ADT/APInt.h>

#include "token.hpp"

namespace Expr {

class Expr {
    public:
        ~Expr() = default;

        virtual void print(int indent = 0) = 0;
        
        void whitespace(int indent, std::string message) {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << message;
        }
};

class Binary : public Expr {
    std::shared_ptr<Expr>  left;
    std::shared_ptr<Token> operand;
    std::shared_ptr<Expr>  right;

    public:
        Binary(std::shared_ptr<Expr> left, std::shared_ptr<Token> operand, std::shared_ptr<Expr> right) : left(std::move(left)), operand(std::move(operand)), right(std::move(right)) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Binary(\n";
            this->left->print(indent + 4);
            for (int i = 1; i <= indent + 4; i++)
                std::cout << " "; std::cout << "Token(" 
                                            << this->operand->lexeme 
                                            << ")\n";
            this->right->print(indent + 4);
            for (int i = 1; i <= indent; i++)
                std::cout << " ";
            std::cout << ")\n";
        }

        /*llvm::Value* codegen(std::shared_ptr<llvm::LLVMContext> context) {
            return llvm::Constant
        }*/
};

class Suffix : public Expr {
    std::shared_ptr<Expr>  left;
    std::shared_ptr<Token> operand;

    public:
        Suffix(std::shared_ptr<Expr> left, std::shared_ptr<Token> operand) : left(std::move(left)), operand(std::move(operand)) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Suffix(\n";
            this->left->print(indent + 4);
            for (int i = 1; i <= indent + 4; i++)
                std::cout << " "; std::cout << "Token(" << this->operand->lexeme << ")\n";
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << ")\n";
        }
};

class Prefix : public Expr {
    std::shared_ptr<Expr>  right;
    std::shared_ptr<Token> operand;

    public:
        Prefix(std::shared_ptr<Expr> right, std::shared_ptr<Token> operand) : right(std::move(right)), operand(std::move(operand)) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Prefix(\n";
            this->right->print(indent + 4);
            for (int i = 1; i <= indent + 4; i++)
                std::cout << " "; std::cout << "Token(" << this->operand->lexeme << ")\n";
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << ")\n";
        }
};

class Type : public Expr {
    std::shared_ptr<Token> type;

    public:
        Type(std::shared_ptr<Token> type) : type(std::move(type)) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Type(" << this->type->lexeme << ")\n";
        }
};

class Scope : public Expr {
    std::shared_ptr<Expr> root;
    std::shared_ptr<Expr> member;

    public:
        Scope(std::shared_ptr<Expr> root, std::shared_ptr<Expr> member) 
          : root(std::move(root)), member(std::move(member)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Expr.Scope(\n");
            this->root->print(indent + 4);
            this->member->print(indent + 4);
            this->whitespace(indent, ")\n");
        }
};

class Call : public Expr {
    std::shared_ptr<Expr> name;
    std::vector<std::shared_ptr<Expr>> args;

    public:
        Call(std::shared_ptr<Expr> name, std::vector<std::shared_ptr<Expr>> args) 
          : name(std::move(name)), args(args) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Expr.Call(\n");
            this->name->print(indent + 4);

            if (this->args.size() > 0) {
                this->whitespace(indent + 4, "Args(\n");
                for (auto& arg : this->args)
                    arg->print(indent + 8);
                this->whitespace(indent + 4, ")\n");
            }

            this->whitespace(indent, ")\n");
        }
};

class Grouping : public Expr {
    std::shared_ptr<Expr> expression;

    public:
        Grouping(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Grouping(\n";
            this->expression->print(indent + 4);
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << ")\n";
        }
};

class Nil : public Expr {
    public:
        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Literal(nil)\n";
        }
};

class FloatLit : public Expr {
    double value;

    public:
        FloatLit(double value) : value(value) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Literal(" << this->value << ")\n";
        }
        //llvm::Value* codegen(std::shared_ptr<llvm::LLVMContext> context) {
        //    return llvm::ConstantInt(context, llvm::Type::getInt32Ty(*context.get()), llvm::APInt(*this->value.get()));
        //}
};

class IntLit : public Expr {
    int value;

    public:
        IntLit(int value) : value(value) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Literal(" << this->value << ")\n";
        }
        //llvm::Value* codegen(std::shared_ptr<llvm::LLVMContext> context) {
        //    return llvm::ConstantInt(context, llvm::Type::getInt32Ty(*context.get()), llvm::APInt(*this->value.get()));
        //}
};

class StringLit : public Expr {
    std::string value;

    public:
        StringLit(std::string value) : value(value) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Literal(\"" << this->value << "\")\n";
        }

        void guh() {
            std::vector<char> chars = {};
            for (const char character : this->value)
                chars.push_back(character);
        }
};

class Variable : public Expr {
    std::string name;

    public:
        Variable(std::string name) : name(name) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Variable(" << this->name << ")\n";
        }
};
}


namespace Stmt {
class Stmt {
    public:
        ~Stmt() = default;
        virtual void print(int indent = 0) = 0;

        void whitespace(int indent, std::string message) {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << message;
        }
};

class Expression : public Stmt {
    std::shared_ptr<Expr::Expr> expression;

    public:
        Expression(std::shared_ptr<Expr::Expr> expression) : expression(std::move(expression)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Expression(\n");
            this->expression->print(indent + 4);
            this->whitespace(indent, ")\n");
        }
};

class Reassign : public Stmt {
    std::shared_ptr<Token>      name;
    std::shared_ptr<Expr::Expr> value;

    public:
        Reassign(std::shared_ptr<Token> name, std::shared_ptr<Expr::Expr> value) 
            : name(std::move(name)), value(std::move(value)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Reassign(\n");
            this->whitespace(indent + 4, "Name(" + name->lexeme + ")\n");
            this->value->print(indent + 4);
            this->whitespace(indent, ")\n");
        }
};

class Mutable : public Stmt {
    std::shared_ptr<Token>                   name;
    std::vector<std::shared_ptr<Expr::Expr>> types;
    std::shared_ptr<Expr::Expr>              value;

    bool is_static;

    public:
        Mutable(std::shared_ptr<Token> name, bool is_static, std::vector<std::shared_ptr<Expr::Expr>> types, std::shared_ptr<Expr::Expr> value) 
            : name(std::move(name)), is_static(is_static), types(types), value(std::move(value)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Mutable(\n");
            this->whitespace(indent + 4, "Name(" + name->lexeme + ")\n");

            if (this->is_static)
                this->whitespace(indent + 4, "Static(true)\n");
            else
                this->whitespace(indent + 4, "Static(false)\n");

            if (this->types.size() != 0) {
                this->whitespace(indent + 4, "Types(\n");
                for (std::shared_ptr<Expr::Expr> &type : this->types)
                    type->print(indent + 8);
                this->whitespace(indent + 4, ")\n");
            }
            this->value->print(indent + 4);
            this->whitespace(indent, ")\n");
        }
};

class Constant : public Stmt {
    std::shared_ptr<Token>                   name;
    std::vector<std::shared_ptr<Expr::Expr>> types;
    std::shared_ptr<Expr::Expr>              value;

    public:
        Constant(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Expr::Expr>> types, std::shared_ptr<Expr::Expr> value) 
            : name(std::move(name)), types(types), value(std::move(value)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Constant(\n");
            this->whitespace(indent + 4,"Name(" + name->lexeme + ")\n");
            if (this->types.size() != 0) {
                this->whitespace(indent + 4, "Types(\n");    
                for (auto &type : this->types)
                    type->print(indent + 8);
                this->whitespace(indent + 4, ")\n");
            }
            this->value->print(indent + 4);
            this->whitespace(indent, ")\n");
        }
};

class If : public Stmt {
    std::shared_ptr<Expr::Expr> conditional;
    std::shared_ptr<Stmt>       then_branch;
    std::shared_ptr<Stmt>       else_branch;

    public:
        If(std::shared_ptr<Expr::Expr> conditional, std::shared_ptr<Stmt> then_branch, std::shared_ptr<Stmt> else_branch)
          : conditional(std::move(conditional)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.If(\n");
            this->conditional->print(indent + 4);
            this->then_branch->print(indent + 4);
            if (this->else_branch != nullptr)
                this->else_branch->print(indent + 4);
            this->whitespace(indent, ")\n");
        }
};

class While : public Stmt {
    std::shared_ptr<Expr::Expr> conditional;
    std::shared_ptr<Stmt> body;

    public:
        While(std::shared_ptr<Expr::Expr> conditional, std::shared_ptr<Stmt> body)
          : conditional(std::move(conditional)), body(std::move(body)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.While(\n");
            this->conditional->print(indent + 4);
            this->body->print(indent + 4);
            this->whitespace(indent, ")\n");
        }
};

class Block : public Stmt {
    std::vector<std::shared_ptr<Stmt>> statements;

    public:
        Block(std::vector<std::shared_ptr<Stmt>> statements)
          : statements(std::move(statements)) {}


        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Block(\n");
            for (std::shared_ptr<Stmt> &statement : this->statements)
                statement->print(indent + 4);
            this->whitespace(indent, ")\n");
        }
};

class CFor : public Stmt {
    std::shared_ptr<Stmt>       variable;
    std::shared_ptr<Expr::Expr> conditional;
    std::shared_ptr<Expr::Expr> iterable;
    std::shared_ptr<Stmt>       body;

    public:
        CFor(std::shared_ptr<Stmt> variable, std::shared_ptr<Expr::Expr> conditional, std::shared_ptr<Expr::Expr> iterable, std::shared_ptr<Stmt> body)
          : variable(std::move(variable)), conditional(std::move(conditional)), iterable(std::move(iterable)), body(std::move(body)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.For(\n");
            this->variable->print(indent + 4);
            this->conditional->print(indent + 4);
            this->iterable->print(indent + 4);
            this->body->print(indent + 4);
            this->whitespace(indent, ")\n");
        }
};

class FinnFor : public Stmt {
    std::shared_ptr<Expr::Expr> name;
    std::vector<std::shared_ptr<Expr::Expr>> types;
    std::shared_ptr<Expr::Expr> iterator;
    std::shared_ptr<Stmt> body;

    public:
        FinnFor(std::shared_ptr<Expr::Expr> name, std::vector<std::shared_ptr<Expr::Expr>> types, std::shared_ptr<Expr::Expr> iterator, std::shared_ptr<Stmt> body)
          : name(std::move(name)), types(types), iterator(std::move(iterator)), body(std::move(body)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.For(\n");
            this->name->print(indent + 4);
            this->whitespace(indent + 4, "Types(\n");
            for (auto &type : types)
                type->print(indent + 8);
            this->whitespace(indent + 4, ")\n");
            this->iterator->print(indent + 4);
            this->body->print(indent + 4);
            this->whitespace(indent, ")\n");
        }
};

class Func : public Stmt {
    std::shared_ptr<Token> name;
    std::vector<std::shared_ptr<Stmt>> args;
    std::vector<std::shared_ptr<Stmt>> return_types, throw_types;
    std::shared_ptr<Stmt> body;

    public:
        Func(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Stmt>> args, std::vector<std::shared_ptr<Stmt>> return_types, std::vector<std::shared_ptr<Stmt>> throw_types, std::shared_ptr<Stmt> body)
          : name(std::move(name)), args(args), return_types(return_types), throw_types(throw_types), body(std::move(body)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Func(\n");
            this->whitespace(indent + 4, "Expr.Variable(" + this->name->lexeme + ")\n");

            if (this->args.size() > 0) {
                this->whitespace(indent + 4, "Args(\n");
                for (auto& arg : this->args)
                    arg->print(indent + 8);
                this->whitespace(indent + 4, ")\n");
            }

            if (this->return_types.size() > 0) {
                this->whitespace(indent + 4, "Types(\n");
                for (auto& arg : this->args)
                    arg->print(indent + 8);
                this->whitespace(indent + 4, ")\n");
            }

            if (this->throw_types.size() > 0) {
                this->whitespace(indent + 4, "Types(\n");
                for (auto& arg : this->args)
                    arg->print(indent + 8);
                this->whitespace(indent + 4, ")\n");
            }

            this->body->print(indent + 4);
            
            this->whitespace(indent, ")\n");
        }
};

}

#endif
