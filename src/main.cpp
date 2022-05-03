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
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
// #include "llvm/Passes/OptimizationLevel.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"

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

    // Initialize the pass manager.
    llvm::LoopAnalysisManager lam;
    llvm::FunctionAnalysisManager fam;
    llvm::CGSCCAnalysisManager cgam;
    llvm::ModuleAnalysisManager mam;

    llvm::PassBuilder pb;

    // Register all the basic analyses with the managers.
    pb.registerModuleAnalyses(mam);
    pb.registerCGSCCAnalyses(cgam);
    pb.registerFunctionAnalyses(fam);
    pb.registerLoopAnalyses(lam);
    pb.crossRegisterProxies(lam, fam, cgam, mam);

    // Optimize the IR.
    llvm::FunctionPassManager fpm = pb.buildFunctionSimplificationPipeline(llvm::PassBuilder::OptimizationLevel::O2,
                                                                           llvm::PassBuilder::ThinLTOPhase::None);

    fpm.run(*gen_pass.mod->getFunction("main"), fam);

    gen_pass.mod->print(llvm::errs(), nullptr);

    return 0;
}