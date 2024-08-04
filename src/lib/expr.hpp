#ifndef EXPR_HPP
#define EXPR_HPP

#pragma once

#include <iostream>
#include <memory>
#include <map>
#include <string>

#include "token.hpp"
#include "compiler.hpp"

namespace Expr {

class Expr {
    std::string id;

    public:
        ~Expr() = default;

        virtual void print(int indent = 0) = 0;
        virtual std::string dump(int indent = 0) = 0;

        virtual llvm::Value* codegen(Compiler* compiler) = 0;

        std::string add_whitespace(int indent, std::string contents, std::string root) {
            for (int i = 1; i <= indent; i++)
                root += " "; root += contents;
            return root;
        }
        
        void whitespace(int indent, std::string message) {
            if (indent == -1) {
                std::cout << message;
                return;
            }
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << message;
        }
};

class Binary : public Expr {
    std::string id = "Expr.Binary";

    std::shared_ptr<Expr>  left;
    std::shared_ptr<Token> operand;
    std::shared_ptr<Expr>  right;

    public:
        Binary(std::shared_ptr<Expr> left, std::shared_ptr<Token> operand, std::shared_ptr<Expr> right) : left(std::move(left)), operand(std::move(operand)), right(std::move(right)) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Binary(\n";
            this->left->print(indent + 2);
            for (int i = 1; i <= indent + 2; i++)
                std::cout << " "; std::cout << "Token(" 
                                            << this->operand->lexeme 
                                            << ")\n";
            this->right->print(indent + 2);
            for (int i = 1; i <= indent; i++)
                std::cout << " ";
            std::cout << ")\n";
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Expr.Binary(\n", root);
            root += this->left->dump(indent + 2);
            root += this->operand->dump(indent + 2);
            root += this->right->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }
        
        llvm::Value* codegen(Compiler* compiler) override {
            llvm::Value* left_value = this->left->codegen(compiler);
            llvm::Value* right_value = this->right->codegen(compiler);

            if (!(left_value || right_value))
                return nullptr;
            
            switch (this->operand->token_type) {

                case TokenType::PLUS: {
                    break;
                    //return compiler->builder->CreateFAdd(left_value, right_value, "addtmp");
                }

                default: {
                    break;
                }

            }

            //return;
        }
};

class Suffix : public Expr {
    std::string id = "Expr.Suffix";

    std::shared_ptr<Expr>  left;
    std::shared_ptr<Token> operand;

    public:
        Suffix(std::shared_ptr<Expr> left, std::shared_ptr<Token> operand) : left(std::move(left)), operand(std::move(operand)) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Suffix(\n";
            this->left->print(indent + 2);
            for (int i = 1; i <= indent + 2; i++)
                std::cout << " "; std::cout << "Token(" << this->operand->lexeme << ")\n";
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << ")\n";
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Expr.Suffix(\n", root);
            root += this->left->dump(indent + 2);
            root += this->operand->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }

        llvm::Value* codegen(Compiler* compiler) override {}
};

class Prefix : public Expr {
    std::string id = "Expr.Prefix";

    std::shared_ptr<Expr>  right;
    std::shared_ptr<Token> operand;

    public:
        Prefix(std::shared_ptr<Expr> right, std::shared_ptr<Token> operand) : right(std::move(right)), operand(std::move(operand)) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Prefix(\n";
            this->right->print(indent + 2);
            for (int i = 1; i <= indent + 2; i++)
                std::cout << " "; std::cout << "Token(" << this->operand->lexeme << ")\n";
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << ")\n";
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Expr.Prefix(\n", root);
            root += this->right->dump(indent + 2);
            root += this->operand->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }
        
        llvm::Value* codegen(Compiler* compiler) override {}
};

class Scope : public Expr {
    std::string id = "Expr.Scope";

    std::shared_ptr<Expr> root;
    std::shared_ptr<Expr> member;

    public:
        Scope(std::shared_ptr<Expr> root, std::shared_ptr<Expr> member) 
          : root(std::move(root)), member(std::move(member)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Expr.Scope(\n");
            this->root->print(indent + 2);
            this->member->print(indent + 2);
            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Expr.Scope(\n", root);
            root += this->root->dump(indent + 2);
            root += this->member->dump(indent + 2);            
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }

        llvm::Value* codegen(Compiler* compiler) override {}
};

class Call : public Expr {
    std::string id = "Expr.Call";

    std::shared_ptr<Expr> name;
    std::vector<std::shared_ptr<Expr>> args;

    public:
        Call(std::shared_ptr<Expr> name, std::vector<std::shared_ptr<Expr>> args) 
          : name(std::move(name)), args(args) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Expr.Call(\n");
            this->name->print(indent + 2);

            if (this->args.size() > 0) {
                this->whitespace(indent + 2, "Args(\n");
                for (auto& arg : this->args)
                    arg->print(indent + 4);
                this->whitespace(indent + 2, ")\n");
            }

            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Expr.Call(\n", root);
            root += this->name->dump(indent + 2);
            root = this->add_whitespace(indent + 2, "Args(\n", root);
            for (auto& arg : this->args)
                root += arg->dump(indent + 4);
            root = this->add_whitespace(indent + 2, ")\n", root);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }

        llvm::Value* codegen(Compiler* compiler) override {}
};

class Grouping : public Expr {
    std::string id = "Expr.Grouping";

    std::shared_ptr<Expr> expression;

    public:
        Grouping(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Grouping(\n";
            this->expression->print(indent + 2);
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << ")\n";
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Expr.Grouping(\n", root);
            root += this->expression->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }

        llvm::Value* codegen(Compiler* compiler) override {}
};

class Nil : public Expr {
    std::string id = "Expr.Nil";

    public:
        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Literal(nil)\n";
        }

        std::string dump(int indent = 0) override {
            std::string root = "";
            root = this->add_whitespace(indent, "Expr.Literal(nil)\n", root);
            return root;
        }

        llvm::Value* codegen(Compiler* compiler) override {}
};

class FloatLit : public Expr {
    std::string id = "Expr.FloatLit";

    double value;

    public:
        FloatLit(double value) : value(value) {}

        void print(int indent = 0) override {

            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Literal(" << this->value << ")\n";
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Expr.Literal(" + std::to_string(this->value) + ")\n", root);

            return root;
        }

        llvm::Value* codegen(Compiler* compiler) override {}
};

class IntLit : public Expr {
    std::string id = "Expr.IntLit";

    int value;

    public:
        IntLit(int value) : value(value) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Literal(" << this->value << ")\n";
        }

        std::string dump(int indent = 0) override {
            std::string root = "";
            return root;
        }

        llvm::Value* codegen(Compiler* compiler) override {}
};

class BoolLit : public Expr {
    std::string id = "Expr.BoolLit";
    
    bool value;

    public:
        BoolLit(bool value) : value(value) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Expr.Literal(");
            if (this->value)
                this->whitespace(-1, "true)\n");
            else
                this->whitespace(-1, "false)\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            if (this->value)
                root = this->add_whitespace(indent, "Expr.Literal(true)\n", root);
            else
                root = this->add_whitespace(indent, "Expr.Literal(false)\n", root);
            
            return root;
        }

        llvm::Value* codegen(Compiler* compiler) override {}
};

class StringLit : public Expr {
    std::string id = "Expr.StringLit";

    std::string value;

    public:
        StringLit(std::string value) : value(value) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Literal(\"" << this->value << "\")\n";
        }

        std::string dump(int indent = 0) override {
            std::string root = "";
            
            root = this->add_whitespace(indent, "Expr.Literal(" + this->value + ")\n", root);

            return root;
        }

        llvm::Value* codegen(Compiler* compiler) override {}
};

class Type : public Expr {
    std::string id = "Expr.Type";

    std::shared_ptr<Token> type;

    public:
        Type(std::shared_ptr<Token> type)
          : type(std::move(type)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Expr.Type(" + this->type->lexeme + ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Expr.Type(\n", root);
            root += this->type->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }

        llvm::Value* codegen(Compiler* compiler) override {}
};

class Variable : public Expr {
    std::string id = "Expr.Variable";

    std::string name;

    public:
        Variable(std::string name) : name(name) {}

        void print(int indent = 0) override {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << "Expr.Variable(" << this->name << ")\n";
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Expr.Variable(" + this->name + ")\n", root);

            return root;
        }

        llvm::Value* codegen(Compiler* compiler) override {}
};

class Reassign : public Expr {
    std::string id = "Expr.Reassign";

    std::shared_ptr<Expr> name;
    std::shared_ptr<Expr> value;

    public:
        Reassign(std::shared_ptr<Expr> name, std::shared_ptr<Expr> value) 
            : name(std::move(name)), value(std::move(value)) {}

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Expr.Reassign(\n", root);
            root += this->name->dump(indent + 2);
            root += this->value->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }

        void print(int indent = 0) override {
            this->whitespace(indent, "Expr.Reassign(\n");
            this->name->print(indent + 2);
            this->value->print(indent + 2);
            this->whitespace(indent, ")\n");
        }

        llvm::Value* codegen(Compiler* compiler) override {}
};

}


namespace Stmt {
class Stmt {

    public:
        std::string id;
        ~Stmt() = default;

        virtual void print(int indent = 0) = 0;
        virtual std::string dump(int indent = 0) = 0;

        std::string add_whitespace(int indent, std::string message, std::string root) {
            for (int i = 0; i <= indent; i++)
                root += " "; root += message;
            return root;
        }

        void whitespace(int indent, std::string message) {
            for (int i = 1; i <= indent; i++)
                std::cout << " "; std::cout << message;
        }
};

class Expression : public Stmt {
    std::string id = "Stmt.Expression";

    std::shared_ptr<Expr::Expr> expression;

    public:
        Expression(std::shared_ptr<Expr::Expr> expression) : expression(std::move(expression)) {}

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Stmt.Expression(\n", root);
            root += this->expression->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Expression(\n");
            this->expression->print(indent + 2);
            this->whitespace(indent, ")\n");
        }
};

class Mutable : public Stmt {
    std::string id = "Stmt.Mutable";

    std::shared_ptr<Token>                   name;
    std::vector<std::shared_ptr<Expr::Expr>> types;
    std::shared_ptr<Expr::Expr>              value;

    public:
        Mutable(std::shared_ptr<Token> name,  std::vector<std::shared_ptr<Expr::Expr>> types, std::shared_ptr<Expr::Expr> value) 
            : name(std::move(name)), types(types), value(std::move(value)) {}

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Stmt.Mutable(\n", root);
            root += this->name->dump(indent + 2);
            root = this->add_whitespace(indent + 2, "Types(\n", root);
            for (auto& type : this->types)
                root += type->dump(indent + 4);
            root = this->add_whitespace(indent + 2, ")\n", root);
            root += this->value->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Mutable(\n");
            this->whitespace(indent + 2, "Name(" + name->lexeme + ")\n");

            if (this->types.size() != 0) {
                this->whitespace(indent + 2, "Types(\n");
                for (std::shared_ptr<Expr::Expr>& type : this->types)
                    type->print(indent + 4);
                this->whitespace(indent + 2, ")\n");
            }

            if (this->value != nullptr)
                this->value->print(indent + 2);

            this->whitespace(indent, ")\n");
        }
};

class Constant : public Stmt {
    std::string id = "Stmt.Constant";

    std::shared_ptr<Token>                   name;
    std::vector<std::shared_ptr<Expr::Expr>> types;
    std::shared_ptr<Expr::Expr>              value;

    public:
        Constant(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Expr::Expr>> types, std::shared_ptr<Expr::Expr> value) 
            : name(std::move(name)), types(types), value(std::move(value)) {}

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Stmt.Constant(\n", root);
            root += this->name->dump(indent + 2);
            root = this->add_whitespace(indent + 2, "Types(\n", root);
            for (auto& type : this->types)
                root += type->dump(indent + 4);
            root = this->add_whitespace(indent + 2, ")\n", root);
            root += this->value->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Constant(\n");
            this->whitespace(indent + 2,"Name(" + name->lexeme + ")\n");
            if (this->types.size() != 0) {
                this->whitespace(indent + 2, "Types(\n");    
                for (auto &type : this->types)
                    type->print(indent + 4);
                this->whitespace(indent + 2, ")\n");
            }
            this->value->print(indent + 2);
            this->whitespace(indent, ")\n");
        }
};

class If : public Stmt {
    std::string id = "Stmt.If";

    std::shared_ptr<Expr::Expr> conditional;
    std::shared_ptr<Stmt>       then_branch;
    std::shared_ptr<Stmt>       else_branch;

    public:
        If(std::shared_ptr<Expr::Expr> conditional, std::shared_ptr<Stmt> then_branch, std::shared_ptr<Stmt> else_branch)
          : conditional(std::move(conditional)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.If(\n");
            this->conditional->print(indent + 2);
            this->then_branch->print(indent + 2);
            if (this->else_branch != nullptr)
                this->else_branch->print(indent + 2);
            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Stmt.If(\n", root);
            root += this->conditional->dump(indent + 2);
            root += this->then_branch->dump(indent + 2);
            if (this->else_branch != nullptr)
                root += this->else_branch->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }
};

class While : public Stmt {
    std::string id = "Stmt.While";

    std::shared_ptr<Expr::Expr> conditional;
    std::shared_ptr<Stmt> body;

    public:
        While(std::shared_ptr<Expr::Expr> conditional, std::shared_ptr<Stmt> body)
          : conditional(std::move(conditional)), body(std::move(body)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.While(\n");
            this->conditional->print(indent + 2);
            this->body->print(indent + 2);
            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Stmt.While(\n", root);
            root += this->conditional->dump(indent + 2);
            root += this->body->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }
};

class Block : public Stmt {
    std::string id = "Stmt.Block";

    std::vector<std::shared_ptr<Stmt>> statements;

    public:
        Block(std::vector<std::shared_ptr<Stmt>> statements)
          : statements(std::move(statements)) {}


        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Block(\n");
            for (std::shared_ptr<Stmt> &statement : this->statements)
                statement->print(indent + 2);
            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Stmt.Block(\n", root);
            for (auto& statement : this->statements)
                root += statement->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }
};

class CFor : public Stmt {
    std::string id = "Stmt.CFor";

    std::shared_ptr<Stmt>       variable;
    std::shared_ptr<Expr::Expr> conditional;
    std::shared_ptr<Expr::Expr> iterable;
    std::shared_ptr<Stmt>       body;

    public:
        CFor(std::shared_ptr<Stmt> variable, std::shared_ptr<Expr::Expr> conditional, std::shared_ptr<Expr::Expr> iterable, std::shared_ptr<Stmt> body)
          : variable(std::move(variable)), conditional(std::move(conditional)), iterable(std::move(iterable)), body(std::move(body)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.For(\n");
            this->variable->print(indent + 2);
            this->conditional->print(indent + 2);
            this->iterable->print(indent + 2);
            this->body->print(indent + 2);
            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Stmt.For(\n", root);
            root += this->variable->dump(indent + 2);
            root += this->conditional->dump(indent + 2);
            root += this->iterable->dump(indent + 2);
            root += this->body->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }
};

class FinnFor : public Stmt {
    std::string id = "Stmt.FinnFor";

    std::shared_ptr<Expr::Expr> name;
    std::vector<std::shared_ptr<Expr::Expr>> types;
    std::shared_ptr<Expr::Expr> iterator;
    std::shared_ptr<Stmt> body;

    public:
        FinnFor(std::shared_ptr<Expr::Expr> name, std::vector<std::shared_ptr<Expr::Expr>> types, std::shared_ptr<Expr::Expr> iterator, std::shared_ptr<Stmt> body)
          : name(std::move(name)), types(types), iterator(std::move(iterator)), body(std::move(body)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.For(\n");
            this->name->print(indent + 2);
            this->whitespace(indent + 2, "Types(\n");
            for (auto &type : this->types)
                type->print(indent + 4);
            this->whitespace(indent + 2, ")\n");
            this->iterator->print(indent + 2);
            this->body->print(indent + 2);
            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Stmt.For(\n", root);
            root += this->name->dump(indent + 2);
            root = this->add_whitespace(indent + 2, "Types(\n", root);
            for (auto& type : this->types)
                root += type->dump(indent + 4);
            root = this->add_whitespace(indent + 2, ")\n", root);
            root += this->iterator->dump(indent + 2);
            root += this->body->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }
};

class Interface : public Stmt {
    std::string id = "Stmt.Interface";

    std::shared_ptr<Token> name;
    std::vector<std::shared_ptr<Stmt>> body;

    public:
        Interface(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Stmt>> body) 
          : name(std::move(name)), body(body) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Interface(\n");
            this->whitespace(indent + 2, "Expr.Variable(" + this->name->lexeme + ")\n");
            for (auto& statement : this->body)
                statement->print(indent + 2);
            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Stmt.Interface(\n", root);
            root += this->name->dump(indent + 2);
            for (auto& statement : this->body)
                root += statement->dump(indent + 2);
            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }
};

class Enum : public Stmt {
    std::string id = "Stmt.Enum";

    std::shared_ptr<Token>                   name;
    std::vector<std::shared_ptr<Expr::Expr>> types;
    std::vector<std::shared_ptr<Stmt>>       body;

    public:
        Enum(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Expr::Expr>> types, std::vector<std::shared_ptr<Stmt>> body)
          : name(std::move(name)), types(std::move(types)), body(body) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Enum(\n");
            this->whitespace(indent + 2, "Expr.Variable(" + this->name->lexeme + ")\n");

            for (auto& type : this->types)
                type->print(indent + 2);

            for (auto& statement : this->body)
                statement->print(indent + 2);

            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            root = this->add_whitespace(indent, "Stmt.Enum(\n", root);

            root += this->name->dump(indent + 2);

            for (auto& type : this->types)
                root += type->dump(indent + 2);
                
            for (auto& statement : this->body)
                root += statement->dump(indent + 2);

            root = this->add_whitespace(indent, ")\n", root);

            return root;
        }
};

class Func : public Stmt {
    std::string id = "Stmt.Func";

    std::shared_ptr<Expr::Expr> name;
    std::vector<std::shared_ptr<Stmt>> args;
    std::vector<std::shared_ptr<Expr::Expr>> return_types, throw_types;
    std::shared_ptr<Stmt> body;

    public:
        Func(std::shared_ptr<Expr::Expr> name, std::vector<std::shared_ptr<Stmt>> args, std::vector<std::shared_ptr<Expr::Expr>> return_types, std::vector<std::shared_ptr<Expr::Expr>> throw_types, std::shared_ptr<Stmt> body)
          : name(std::move(name)), args(args), return_types(return_types), throw_types(throw_types), body(std::move(body)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Func(\n");
            this->name->print(indent + 2);

            if (this->args.size() > 0) {
                this->whitespace(indent + 2, "Args(\n");
                for (auto& arg : this->args)
                    arg->print(indent + 4);
                this->whitespace(indent + 2, ")\n");
            }

            if (this->return_types.size() > 0) {
                this->whitespace(indent + 2, "Types(\n");
                for (auto& type : this->return_types)
                    type->print(indent + 4);
                this->whitespace(indent + 2, ")\n");
            }

            if (this->throw_types.size() > 0) {
                this->whitespace(indent + 2, "Types(\n");
                for (auto& type : this->throw_types)
                    type->print(indent + 4);
                this->whitespace(indent + 2, ")\n");
            }

            this->body->print(indent + 2);
            
            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            return root;
        }
};

class Throw : public Stmt {
    std::string id = "Stmt.Throw";

    std::shared_ptr<Expr::Expr> body;

    public:
        Throw(std::shared_ptr<Expr::Expr> body)
          : body(std::move(body)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Throw(\n");
            this->body->print(indent + 2);
            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            return root;
        }
};

class Return : public Stmt {
    std::string id = "Stmt.Return";

    std::shared_ptr<Expr::Expr> body;

    public:
        Return(std::shared_ptr<Expr::Expr> body)
          : body(std::move(body)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Return(\n");
            this->body->print(indent + 2);
            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            return root;
        }
};

class Struct : public Stmt {
    std::string id = "Stmt.Struct";

    std::shared_ptr<Token> name;
    std::vector<std::shared_ptr<Stmt>> members;

    public:
        Struct(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Stmt>> members) 
          : name(std::move(name)), members(members) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Struct(\n");
            this->whitespace(indent + 2, "Token(" + this->name->lexeme + ")\n");
            for (auto& member : members) 
                member->print(indent + 2);
            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            return root;
        }
};

class Import : public Stmt {
    std::string id = "Stmt.Import";

    std::shared_ptr<Expr::Expr> module;

    public:
        Import(std::shared_ptr<Expr::Expr> module)
          : module(std::move(module)) {}

        void print(int indent = 0) override {
            this->whitespace(indent, "Stmt.Import(\n");
            this->module->print(indent + 2);
            this->whitespace(indent, ")\n");
        }

        std::string dump(int indent = 0) override {
            std::string root = "";

            return root;
        }
};

}

#endif
