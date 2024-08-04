#ifndef COMPILER_HPP
#define COMPILER_HPP

#pragma once

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include "expr.hpp"

class Compiler {

    //std::vector<std::shared_ptr<Stmt::Stmt>> statements;

    public:
        static std::unique_ptr<llvm::LLVMContext>  context;
        static std::unique_ptr<llvm::IRBuilder<>>  builder;
        static std::unique_ptr<llvm::Module>       module;
        static std::map<std::string, llvm::Value*> named_values;
        
        //Compiler(std::vector<std::shared_ptr<Stmt::Stmt>> statements);

};

#endif
