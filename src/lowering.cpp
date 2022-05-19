// ------------------------------------------------------------
//  lowering.cpp
//  
//  Lower the LLVM IR to a given target.
// ------------------------------------------------------------


// Include statements.
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <system_error>

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Program.h"

#include "lowering.h"
#include "bf_error.h"
#include "util.h"


// ------------------------------------------------------------
//  optimize
// 
//  Optimize the LLVM IR.
// ------------------------------------------------------------
void lowering::optimize(unsigned opt_level) {

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

    // Get the optimization level.
    llvm::PassBuilder::OptimizationLevel opt;
    
    switch(opt_level) {
        case 0: opt = llvm::PassBuilder::OptimizationLevel::O0; break;
        case 1: opt = llvm::PassBuilder::OptimizationLevel::O1; break;
        case 2: opt = llvm::PassBuilder::OptimizationLevel::O2; break;
        case 3: opt = llvm::PassBuilder::OptimizationLevel::O3; break;
        default: opt = llvm::PassBuilder::OptimizationLevel::O2;
    }

    // Optimize the IR.
    llvm::FunctionPassManager fpm = pb.buildFunctionSimplificationPipeline(opt, llvm::PassBuilder::ThinLTOPhase::None);

    fpm.run(*mod->getFunction("main"), fam);
}


// ------------------------------------------------------------
//  compile
// 
//  Compile the LLVM IR down to either asm or obj file.
// ------------------------------------------------------------
void lowering::compile(std::string output_file, bool target_asm) {

    // Initialize the output stream.
    std::error_code llvm_ec;
    llvm::raw_fd_ostream dest(output_file, llvm_ec, llvm::sys::fs::OF_None);

    if (llvm_ec) {
        ec = brain_errc::lower_output;
        return;
    }

    // Run the LLVM legacy pass manager to lower the IR.
    llvm::legacy::PassManager pass;
    llvm::CodeGenFileType file_type = target_asm ? llvm::CGFT_AssemblyFile : llvm::CGFT_ObjectFile;

    if (machine->addPassesToEmitFile(pass, dest, nullptr, file_type)) {
        ec = brain_errc::lower_object;
        return;
    }

    pass.run(*mod);
    dest.flush();
}


// ------------------------------------------------------------
//  link
// 
//  Link the object file and produce the final executable.
// ------------------------------------------------------------
void lowering::link(std::string obj_file, std::string exe_file) {

    // Find clang to link the program.
    auto clang = llvm::sys::findProgramByName("clang");

    if (!clang) {
        ec = brain_errc::lower_clang;
        return;
    }

    if (brain::DEBUG) std::cerr << "object file: " << obj_file << ", executible: " << exe_file << std::endl;

    // Set the list of arguments to pass to clang.
    std::vector<llvm::StringRef> clang_args = {clang.get(), "-O2", obj_file, "-o", exe_file};
    
    // Run and wait on the results of clang.
    std::string clang_err;
    auto result = llvm::sys::ExecuteAndWait(clang.get(), llvm::makeArrayRef(clang_args), llvm::NoneType::None, {}, 10, 0, &clang_err);

    if (-1 == result) {
        ec = brain_errc::lower_linking;
        return;
    }
}