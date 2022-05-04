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
#include <filesystem>

#include "llvm/Pass.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"

#include "util.h"
#include "ast_builder.h"
#include "code_gen.h"
#include "cmd_parser.h"


int main(int argc, char** argv) {
    
    // Initialize the command argument parser.
    cmd_parser input(argc, argv);

    // Make sure at least the input file is given, and that it is a valid file.
    if (!input.check_input_file()) {
        brain::print_usage();
        return 2;
    }

    // Read in the file.
    std::ifstream ifs(input.get_input_file());
    std::stringstream bf_prog;
    bf_prog << ifs.rdbuf();

    // Build the AST.
    ast_builder ast_pass(bf_prog.str());
    std::shared_ptr<ast> tree = std::make_shared<ast>(brain::root);
    ast_pass.visit(tree);

    // Initialize the code gen pass and generate the LLVM IR.
    code_gen gen_pass(input.get_input_file());
    bool result = gen_pass.initialize_module();
    
    if (result) gen_pass.visit(tree);
    else return 1;

    // Initialize the pass managers.
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