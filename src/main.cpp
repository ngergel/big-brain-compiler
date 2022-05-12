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
#include <system_error>

#include "llvm/Pass.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/IRReader/IRReader.h"

#include "util.h"
#include "ast_builder.h"
#include "code_gen.h"
#include "cmd_parser.h"
#include "bf_error.h"


int main(int argc, char** argv) {

    // Initialize the command argument parser.
    cmd_parser input(argc, argv);

    // Make sure at least the input file is given, and that it is a valid file.
    if (!input.check_input_file()) {
        std::cerr << brain::err_msg(input.ec.message());
        std::cerr << brain::USAGE;
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

    // Catch the only possible syntax error in bf, unbalanced brackets!
    if (ast_pass.ec != brain_errc::no_err) {
        std::cerr << brain::err_msg(ast_pass.ec.message(), ast_pass.err_node);
        return 1;
    }

    // Initialize the code gen pass and generate the LLVM IR.
    code_gen gen_pass(input.get_input_file());
    gen_pass.initialize_module();
    gen_pass.visit(tree);
    
    if (gen_pass.ec != brain_errc::no_err) {
        std::cerr << brain::err_msg(gen_pass.ec.message());
        return 1;
    }

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
    llvm::FunctionPassManager fpm = pb.buildFunctionSimplificationPipeline(input.get_opt_level(),
                                                                           llvm::PassBuilder::ThinLTOPhase::None);

    fpm.run(*gen_pass.mod->getFunction("main"), fam);

    std::error_code ec;
    llvm::raw_fd_ostream dest("new_test", ec, llvm::sys::fs::OF_None);

    if (ec) {
        llvm::errs() << "Could not open file: " << ec.message();
        return 1;
    }

    llvm::legacy::PassManager pass;
    auto FileType = llvm::CGFT_ObjectFile;
    // llvm::MCContext* mctx;

    // llvm::TargetLibraryInfoImpl TLII(llvm::Triple(gen_pass.mod->getTargetTriple()));
    // pass.add(new llvm::TargetLibraryInfoWrapperPass(TLII));

    // llvm::LLVMTargetMachine &LLVMTM = static_cast<llvm::LLVMTargetMachine &>(*gen_pass.machine);
    // llvm::MachineModuleInfoWrapperPass *MMIWP = new llvm::MachineModuleInfoWrapperPass(&LLVMTM);
    // pass.add(MMIWP);

    // llvm::verifyModule(*gen_pass.mod, &llvm::errs());

    // if (gen_pass.machine->addPassesToEmitMC(pass, mctx, dest)) {
    //     llvm::errs() << "TargetMachine can't emit mc context";
    //     return 1;
    // }

    // const_cast<llvm::TargetLoweringObjectFile *>(gen_pass.machine->getObjFileLowering())->Initialize(MMIWP->getMMI().getContext(), *gen_pass.machine);

    if (gen_pass.machine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        llvm::errs() << "TargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(*gen_pass.mod);
    dest.flush();


    // llvm::InitializeNativeTarget();
    // std::string err_str;

    // llvm::EngineBuilder builder;//(std::move(gen_pass.mod));
    // builder.setMCJITMemoryManager(std::make_unique<llvm::SectionMemoryManager>());
    // builder.setOptLevel(llvm::CodeGenOpt::Level::Default);
    // builder.setErrorStr(&err_str);
    // auto executionEngine = builder.create();

    // if (!executionEngine) {
    //     std::cerr << err_str << "\n";
    // } else {
    //     std::cerr << "creation successful\n";
    // }
    // executionEngine->generateCodeForModule(&*gen_pass.mod);

    // llvm::Module* m = gen_pass.mod.release();

    // executionEngine->Modules[0]->dump();
    // executionEngine->addModule(gen_pass.mod);
    // executionEngine->generateCodeForModule(m);
    // executionEngine->finalizeObject();
    // auto res = executionEngine->runFunction("main");
    // res->dump();


    // gen_pass.mod->print(llvm::errs(), nullptr);

    return 0;
}