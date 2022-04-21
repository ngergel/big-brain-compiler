// ------------------------------------------------------------
//  code_gen.cpp
//  
//  Code generation for LLVM IR.
// ------------------------------------------------------------


// Include statements.
#include <cassert>
#include <iostream>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "code_gen.h"
#include "ast.h"


// ------------------------------------------------------------
//  initialize_context
// 
//  Initialize the LLVM context, module, and builder.
// ------------------------------------------------------------
void code_gen::initialize_context() {
    
    // Open a new context and module.
    context = std::make_unique<llvm::LLVMContext>();
    mod = std::make_unique<llvm::Module>(file_name, *context);

    // Create a new builder for the module.
    builder = std::make_unique<llvm::IRBuilder<> >(*context);
}


// ------------------------------------------------------------
//  visit
// 
//  General visit function for code gen.
// ------------------------------------------------------------
void ast_builder::visit(std::shared_ptr<ast>& t) {

    // Make sure we aren't visiting a nil token in the AST.
    if (brain::DEBUG) assert(t->token != brain::nil);

    // Reduce into each of the possible cases.
    switch (t->token) {
        case brain::plus:
        case brain::minus:
        case brain::period:
        case brain::comma:
        case brain::larrow:
        case brain::rarrow:
            visit_single(t);
            break;
        case brain::root:
        case brain::loop:
            visit_multi(t);
            break;
        default:
            t = std::make_shared<ast>(brain::nil);
    }
}