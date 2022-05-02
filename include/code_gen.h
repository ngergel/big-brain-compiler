// ------------------------------------------------------------
//  code_gen.h
//  
//  This pass generates the LLVM IR from the AST.
// ------------------------------------------------------------

#pragma once


// Include statements.
#include <memory>
#include <string>
#include <vector>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/NoFolder.h"
#include "llvm/ADT/Triple.h"

#include "util.h"
#include "ast.h"


class code_gen {
public:

    // File name for the program, and the vector/index consisting of every cell.
    std::string file_name;
    std::vector<llvm::Value*> cells = std::vector<llvm::Value*>(65536, nullptr);
    uint16_t idx = 0;

    // The LLVM context and module to be referenced.
    std::unique_ptr<llvm::LLVMContext> ctx;
    std::unique_ptr<llvm::Module> mod;

    // The target triple and target machine specify what the destination arch will be.
    std::string triple;

    // Function callee's for getchar/putchar, these only need to be defined once.
    llvm::FunctionCallee getchar, putchar;

    // Constructors and deconstructors.
    code_gen() = default;
    code_gen(std::string f): file_name(f) {};

    ~code_gen() = default;

    // Visitor functions for walking the tree and generating LLVM IR.
    void visit(std::shared_ptr<ast>& t);

    void visit_root(std::shared_ptr<ast>& t);
    void visit_plus(std::shared_ptr<ast>& t);
    void visit_minus(std::shared_ptr<ast>& t);
    void visit_period(std::shared_ptr<ast>& t);
    void visit_comma(std::shared_ptr<ast>& t);
    void visit_larrow(std::shared_ptr<ast>& t);
    void visit_rarrow(std::shared_ptr<ast>& t);

    // Initialize the context, module, and builder.
    void initialize_module();

private:

    // The IR builder that this pass uses.
    std::unique_ptr<llvm::IRBuilder<llvm::NoFolder> > builder;
};