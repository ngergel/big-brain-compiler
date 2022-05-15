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
#include "llvm/Support/Alignment.h"
#include "llvm/Analysis/TargetLibraryInfo.h"

#include "code_gen.h"
#include "ast.h"
#include "bf_error.h"


// ------------------------------------------------------------
//  initialize_module
// 
//  Initialize the LLVM context, module, and builder.
// ------------------------------------------------------------
bool code_gen::initialize_module() {
    
    // Open a new context and module.
    ctx = std::make_unique<llvm::LLVMContext>();
    mod = std::make_unique<llvm::Module>(prog_name, *ctx);

    // Create a new builder for the module.
    builder = std::make_unique<llvm::IRBuilder<> >(*ctx);

    // Initialize all targets.
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    // Set the target triple and target machine.
    auto triple = llvm::sys::getDefaultTargetTriple();
    auto target = llvm::TargetRegistry::lookupTarget(triple, llvm_err);

    // Unable to get the target.
    if (!target) {
        ec = brain_errc::gen_bad_init;
        return false;
    }

    llvm::TargetOptions opt;
    auto rm = llvm::Optional<llvm::Reloc::Model>();

    // Set the machine.
    machine = std::unique_ptr<llvm::TargetMachine>(target->createTargetMachine(triple, llvm::sys::getHostCPUName(), "", {}, llvm::Reloc::PIC_));

    // Set the layout and target triple.
    mod->setDataLayout(machine->createDataLayout());
    mod->setTargetTriple(triple);

    return true;
}


// ------------------------------------------------------------
//  visit
// 
//  General visit function for code gen.
// ------------------------------------------------------------
void code_gen::visit(std::shared_ptr<ast>& t) {

    // Make sure we aren't visiting a nil token in the AST.
    if (brain::DEBUG) assert(t->token != brain::nil);

    // Don't proceed if we've already hit an error.
    if (ec != brain_errc::no_err) return;

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
            visit_loop(t);
            break;
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

    // Make sure our token is the root.
    if (brain::DEBUG) assert(t->token == brain::root);

    // Initialize the main function, and its return value.
    llvm::FunctionType* main_ty = llvm::FunctionType::get(builder->getInt32Ty(), std::vector<llvm::Type*>{}, false);
    llvm::Function* main = llvm::Function::Create(main_ty, llvm::Function::ExternalLinkage, "main", *mod);

    // Initialize the entry basic block for main.
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(*ctx, "entry", main);
    builder->SetInsertPoint(entry);

    // Initialize the index and cell array.
    idx = builder->CreateAlloca(builder->getInt16Ty(), 0, "idx");
    builder->CreateStore(builder->getInt16(0), idx);

    llvm::ArrayType* cell_ty = llvm::ArrayType::get(builder->getInt8Ty(), brain::CELL_SIZE);
    cell = builder->CreateAlloca(cell_ty, 0, "cell");
    builder->CreateMemSet(cell, builder->getInt8(0), builder->getInt32(brain::CELL_SIZE), llvm::MaybeAlign(0));

    // Loop through visiting all of the root's children.
    for (std::shared_ptr<ast> c : t->children) visit(c);

    // Create the return statement and validate the generated code.
    builder->CreateRet(builder->getInt32(0));
    llvm::verifyFunction(*main, &llvm::errs());
}


// ------------------------------------------------------------
//  visit_plus
// 
//  Visit a + node, and increment the current cell.
// ------------------------------------------------------------
void code_gen::visit_plus(std::shared_ptr<ast>& t) {

    // Add 1 to the current cell value, allowing for overflow.
    llvm::Value* new_val = builder->CreateAdd(get_cell(), builder->getInt8(1), "add");
    set_cell(new_val);
}


// ------------------------------------------------------------
//  visit_minus
// 
//  Visit a - node, and decrement the current cell.
// ------------------------------------------------------------
void code_gen::visit_minus(std::shared_ptr<ast>& t) {
    
    // Subtract 1 from the current cell value, allowing for overflow.
    llvm::Value* new_val = builder->CreateSub(get_cell(), builder->getInt8(1), "sub");
    set_cell(new_val);
}


// ------------------------------------------------------------
//  visit_period
// 
//  Visit a . node, and print out it's current contents to stdout.
// ------------------------------------------------------------
void code_gen::visit_period(std::shared_ptr<ast>& t) {

    // Initialize the function callee for putchar.
    llvm::FunctionCallee putchar = mod->getOrInsertFunction("putchar", builder->getInt32Ty(), builder->getInt32Ty());

    // Extend the cell to be 32 bits and call putchar.
    llvm::Value* chr = builder->CreateZExt(get_cell(), builder->getInt32Ty(), "zext");
    llvm::CallInst* call = builder->CreateCall(putchar, chr, "putchar_func");
}


// ------------------------------------------------------------
//  visit_comma
// 
//  Visit a , node, and update a cell with the retrieved byte.
// ------------------------------------------------------------
void code_gen::visit_comma(std::shared_ptr<ast>& t) {

    // Initialize the function callee for getchar.
    llvm::FunctionCallee getchar = mod->getOrInsertFunction("getchar", builder->getInt32Ty());

    // Call getchar and then truncate to 8 bits.
    llvm::Value* call = builder->CreateCall(getchar, {}, "getchar_func");
    llvm::Value* new_val = builder->CreateTrunc(call, builder->getInt8Ty(), "trunc");
    set_cell(new_val);
}


// ------------------------------------------------------------
//  visit_larrow
// 
//  Visit a < node, and decrement the index.
// ------------------------------------------------------------
void code_gen::visit_larrow(std::shared_ptr<ast>& t) {

    // Load in the current index and decrement it.
    llvm::Value* idx_val = builder->CreateLoad(idx, "load");
    llvm::Value* new_idx = builder->CreateSub(idx_val, builder->getInt16(1), "decr");

    // Store the new index to memory.
    builder->CreateStore(new_idx, idx);
}


// ------------------------------------------------------------
//  visit_rarrow
// 
//  Visit a > node, and increment the index.
// ------------------------------------------------------------
void code_gen::visit_rarrow(std::shared_ptr<ast>& t) {

    // Load in the current index and increment it.
    llvm::Value* idx_val = builder->CreateLoad(idx, "load");
    llvm::Value* new_idx = builder->CreateAdd(idx_val, builder->getInt16(1), "incr");
    
    // Store the new index to memory.
    builder->CreateStore(new_idx, idx);
}


// ------------------------------------------------------------
//  visit_loop
// 
//  Visit a loop node, and recursively visit it's children.
// ------------------------------------------------------------
void code_gen::visit_loop(std::shared_ptr<ast>& t) {

    // First retrieve the function.
    llvm::Function* main = builder->GetInsertBlock()->getParent();

    // Declare the condition, body, loop and join point basic blocks.
    llvm::BasicBlock* cond = llvm::BasicBlock::Create(*ctx, "cond", main);
    llvm::BasicBlock* body = llvm::BasicBlock::Create(*ctx, "body", main);
    llvm::BasicBlock* join = llvm::BasicBlock::Create(*ctx, "join");

    // Add in a branch from the current BB to the condition BB for fall-through.
    builder->CreateBr(cond);

    // Move the builder to the condition BB and set the comparison.
    builder->SetInsertPoint(cond);
    llvm::Value* cmp = builder->CreateICmpNE(get_cell(), builder->getInt8(0), "cmp");
    builder->CreateCondBr(cmp, body, join);

    // Move the builder and recursively visit the children of the loop.
    builder->SetInsertPoint(body);
    for (std::shared_ptr<ast> c : t->children) visit(c);

    // Once we are done, we fall through to the cond BB.
    builder->CreateBr(cond);

    // Now we insert the join point and finish the loop code generation.
    main->getBasicBlockList().push_back(join);
    builder->SetInsertPoint(join);
}


// ------------------------------------------------------------
//  get_cell
// 
//  Get and return the llvm::Value for the current cell.
// ------------------------------------------------------------
llvm::Value* code_gen::get_cell() {

    // First load the index, then get the ptr for cell[idx].
    llvm::Value* idx_val = builder->CreateLoad(idx, "load");

    llvm::ArrayType* cell_ty = llvm::ArrayType::get(builder->getInt8Ty(), brain::CELL_SIZE);
    llvm::Value* gep = builder->CreateGEP(cell_ty, cell, {builder->getInt64(0), idx_val}, "gep");

    return builder->CreateLoad(gep, "cell");
}


// ------------------------------------------------------------
//  set_cell
// 
//  Set the current cell to the given value.
// ------------------------------------------------------------
void code_gen::set_cell(llvm::Value* val) {
    
    // Make sure that the given value is an i8 value.
    if (brain::DEBUG) assert(val->getType() == builder->getInt8Ty());

    // Then, load the index and get the ptr.
    llvm::Value* idx_val = builder->CreateLoad(idx, "load");

    llvm::ArrayType* cell_ty = llvm::ArrayType::get(builder->getInt8Ty(), brain::CELL_SIZE);
    llvm::Value* gep = builder->CreateGEP(cell_ty, cell, {builder->getInt64(0), idx_val}, "gep");

    // Set the new cell value.
    builder->CreateStore(val, gep);
}