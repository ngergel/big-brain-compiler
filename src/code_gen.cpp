// ------------------------------------------------------------
//  code_gen.cpp
//  
//  Code generation for LLVM IR.
// ------------------------------------------------------------


// Include statements.
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/NoFolder.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/ADT/Triple.h"
#include "llvm/ADT/Optional.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Analysis/TargetLibraryInfo.h"

#include "code_gen.h"
#include "ast.h"


// ------------------------------------------------------------
//  initialize_module
// 
//  Initialize the LLVM context, module, and builder.
// ------------------------------------------------------------
void code_gen::initialize_module() {
    
    // Open a new context and module.
    ctx = std::make_unique<llvm::LLVMContext>();
    mod = std::make_unique<llvm::Module>(file_name, *ctx);

    // Create a new builder for the module.
    builder = std::make_unique<llvm::IRBuilder<llvm::NoFolder> >(*ctx);

    // Initialize the function callee for getchar and putchar.
    getchar = mod->getOrInsertFunction("getchar", builder->getInt32Ty());
    putchar = mod->getOrInsertFunction("putchar", builder->getInt32Ty(), builder->getInt32Ty());

    // // Initialize all targets.
    // llvm::InitializeAllTargetInfos();
    // llvm::InitializeAllTargets();
    // llvm::InitializeAllTargetMCs();
    // llvm::InitializeAllAsmParsers();
    // llvm::InitializeAllAsmPrinters();

    // // Set the target triple and target machine.
    // std::string cpu = "generic", features = "", err;
    // llvm::TargetOptions opt;
    // auto rm = llvm::Optional<llvm::Reloc::Model>();

    // triple = llvm::sys::getDefaultTargetTriple();
    // auto target = llvm::TargetRegistry::lookupTarget(triple, err);
    // // auto target_machine = target->createTargetMachine(mod->getTargetTriple(), cpu, features, opt, rm);


    // if (!target) llvm::errs() << err;
}


// ------------------------------------------------------------
//  visit
// 
//  General visit function for code gen.
// ------------------------------------------------------------
void code_gen::visit(std::shared_ptr<ast>& t) {

    // Make sure we aren't visiting a nil token in the AST.
    if (brain::DEBUG) assert(t->token != brain::nil);

    // Reduce into each of the possible cases.
    switch (t->token) {
        case brain::plus:
            visit_plus(t);
            break;
        case brain::minus:
            visit_minus(t);
            break;
        case brain::period:
            visit_period(t);
            break;
        case brain::comma:
            visit_comma(t);
            break;
        case brain::larrow:
            visit_larrow(t);
            break;
        case brain::rarrow:
            visit_rarrow(t);
            break;
        case brain::root:
            visit_root(t);
            break;
        case brain::loop:
        default:
            t = std::make_shared<ast>(brain::nil);
    }
}


// ------------------------------------------------------------
//  visit_root
// 
//  Visit the root node, and initialize the cells and index.
// ------------------------------------------------------------
void code_gen::visit_root(std::shared_ptr<ast>& t) {

    // Make sure our token is a plus.
    if (brain::DEBUG) assert(t->token == brain::root);

    // Initialize the main function, and its return value.
    llvm::FunctionType* mty = llvm::FunctionType::get(builder->getInt32Ty(), std::vector<llvm::Type*>{}, false);
    llvm::Function* main = llvm::Function::Create(mty, llvm::Function::ExternalLinkage, "main", *mod);

    // Initialize the entry basic block for main.
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(*ctx, "entry", main);
    builder->SetInsertPoint(entry);

    // Initialize each cell in the array to the 0 constant.
    for (size_t i = 0; i < cells.size(); i++) {
        cells[i] = builder->getInt8(0);
    }

    // Loop through visiting all of the root's children.
    for (std::shared_ptr<ast> c : t->children) visit(c);

    // Create the return statement and validate the generated code.
    builder->CreateRet(builder->getInt32(0));
    llvm::verifyFunction(*main);
}


// ------------------------------------------------------------
//  visit_plus
// 
//  Visit a + node, and increment the current cell.
// ------------------------------------------------------------
void code_gen::visit_plus(std::shared_ptr<ast>& t) {

    // Add 1 to the current cell value, allowing for overflow.
    cells[idx] = builder->CreateAdd(cells[idx], builder->getInt8(1), "", false);
}


// ------------------------------------------------------------
//  visit_minus
// 
//  Visit a - node, and decrement the current cell.
// ------------------------------------------------------------
void code_gen::visit_minus(std::shared_ptr<ast>& t) {
    
    // Subtract 1 from the current cell value, allowing for overflow.
    cells[idx] = builder->CreateSub(cells[idx], builder->getInt8(1), "", false);
}


// ------------------------------------------------------------
//  visit_period
// 
//  Visit a . node, and print out it's current contents to stdout.
// ------------------------------------------------------------
void code_gen::visit_period(std::shared_ptr<ast>& t) {

    // Extend the cell to be 32 bits and call putchar.
    llvm::Value* chr = builder->CreateSExt(cells[idx], builder->getInt32Ty(), "");
    llvm::CallInst* call = builder->CreateCall(putchar, chr, "");
}


// ------------------------------------------------------------
//  visit_comma
// 
//  Visit a , node, and update a cell with the retrieved byte.
// ------------------------------------------------------------
void code_gen::visit_comma(std::shared_ptr<ast>& t) {

    // Call getchar and then truncate to 8 bits.
    llvm::Value* call = builder->CreateCall(getchar, {}, "");
    cells[idx] = builder->CreateTrunc(call, builder->getInt8Ty(), "");
}


// ------------------------------------------------------------
//  visit_larrow
// 
//  Visit a < node, and decrement the index.
// ------------------------------------------------------------
void code_gen::visit_larrow(std::shared_ptr<ast>& t) {

    // Decrement the index.
    idx--;
}


// ------------------------------------------------------------
//  visit_rarrow
// 
//  Visit a > node, and increment the index.
// ------------------------------------------------------------
void code_gen::visit_rarrow(std::shared_ptr<ast>& t) {

    // Increment the index.
    idx++;
}