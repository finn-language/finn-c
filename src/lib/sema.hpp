#ifndef SEMA_HPP
#define SEMA_HPP

#include <memory>
#include <vector>
#include <iostream>

#include "expr.hpp"

class SemaAnalyser {
    const std::vector<std::shared_ptr<Stmt::Stmt>> statements;

    public:
        SemaAnalyser(const std::vector<std::shared_ptr<Stmt::Stmt>> statements)
          : statements(statements) {}

        void begin_analysis(void) const {
            for (auto& statement : this->statements)
                this->analyse_statement(&statement);
        }

    private:
        void analyse_statement(const std::shared_ptr<Stmt::Stmt>* statement) const {
            if (statement->id == "")
        }
};

#endif