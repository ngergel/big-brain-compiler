// ------------------------------------------------------------
//  code_gen.h
//  
//  This pass generates the LLVM IR from the AST.
// ------------------------------------------------------------

#pragma once


// Include statements.
#include <memory>
#include <string>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "util.h"
#include "ast.h"


class code_gen {
public:

    // File name, and pointer to the root of the AST.
    std::string file_name;
    std::shared_ptr<ast> root = nullptr;

    // The LLVM context and module to be referenced.
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> mod;

    // Constructors and deconstructors.
    code_gen() = default;
    code_gen(std::string f, std::shared_ptr<ast> r): file_name(f), root(r) {};

    ~code_gen() = default;

    // Visitor functions for walking the tree and generating LLVM IR.
    void visit(std::shared_ptr<ast>& t);

    // Initialize the context, module, and builder.
    void initialize_context();

private:

    // The IR builder that this pass uses.
    std::unique_ptr<llvm::IRBuilder<> > builder;
};