// ------------------------------------------------------------
//  main.cpp
//  
//  Main file and entry point for the compiler.
// ------------------------------------------------------------

// Include statments.
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "llvm/Pass.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/raw_ostream.h"

#include "util.h"
#include "ast_builder.h"
#include "code_gen.h"


int main(int argc, char** argv) {
    
    // Make sure there is the required number of command argments.
    if (argc < 3) {
        std::cout << "Missing required argument.\n"
                  << "Required arguments: <input file path> <output file path>\n";
        return 1;
    }

    // Read in the file.
    std::ifstream ifs(argv[1]);
    std::stringstream bf_prog;
    bf_prog << ifs.rdbuf();

    // Build the AST.
    ast_builder ast_pass(bf_prog.str());
    std::shared_ptr<ast> tree = std::make_shared<ast>(brain::root);
    ast_pass.visit(tree);

    // Initialize the code gen pass and generate the LLVM IR.
    code_gen gen_pass(std::string{argv[1]});
    gen_pass.initialize_module();
    gen_pass.visit(tree);

    // // Initialize the pass manager.
    // auto fpm = std::make_unique<llvm::FunctionPassManager>(gen_pass.mod->get());

    // // Add our passes.
    // fpm->add(llvm::createInstructionCombiningPass());
    // fpm->add(llvm::createReassociatePass());
    // fpm->add(llvm::createGVNPass());
    // fpm->add(llvm::createCFGSimplificationPass());

    // // Do initialization and then run the pass manager.
    // fpm->doInitialization();
    // fpm->run(gen_pass.mod->getFunction("main"));

    gen_pass.mod->print(llvm::errs(), nullptr);

    return 0;
}