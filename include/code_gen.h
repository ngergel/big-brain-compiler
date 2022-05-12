// ------------------------------------------------------------
//  code_gen.h
//  
//  This pass generates the LLVM IR from the AST.
// ------------------------------------------------------------

#pragma once


// Include statements.
#include <memory>
#include <string>
#include <system_error>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Target/TargetMachine.h"

#include "util.h"
#include "ast.h"
#include "bf_error.h"


class code_gen {
public:

    // Error code for the code gen pass.
    std::error_code ec = brain_errc::no_err;
    std::string llvm_err;

    // Program name.
    std::string prog_name;

    // The LLVM context and module to be referenced.
    std::unique_ptr<llvm::LLVMContext> ctx;
    std::unique_ptr<llvm::Module> mod;
    std::unique_ptr<llvm::TargetMachine> machine;

    // Constructors and deconstructors.
    code_gen() = default;
    code_gen(std::string f): prog_name(f) {};

    ~code_gen() = default;

    // Main visitor function for walking the tree and generating LLVM IR.
    void visit(std::shared_ptr<ast>& t);

    // Initialize the context, module, and builder.
    bool initialize_module();

private:

    // The IR builder that this pass uses.
    std::unique_ptr<llvm::IRBuilder<> > builder;

    // Alloca instructions for the head and tape respectively.
    llvm::AllocaInst* idx, *cell;

    // All the token-specific visitor functions.
    void visit_root(std::shared_ptr<ast>& t);
    void visit_plus(std::shared_ptr<ast>& t);
    void visit_minus(std::shared_ptr<ast>& t);
    void visit_period(std::shared_ptr<ast>& t);
    void visit_comma(std::shared_ptr<ast>& t);
    void visit_larrow(std::shared_ptr<ast>& t);
    void visit_rarrow(std::shared_ptr<ast>& t);
    void visit_loop(std::shared_ptr<ast>& t);

    // Helper functions for managing the cell array.
    llvm::Value* get_cell();
    void set_cell(llvm::Value* val);
};