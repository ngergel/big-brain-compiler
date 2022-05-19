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
#include <filesystem>

#include "llvm/Pass.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/Program.h"
#include "llvm/IRReader/IRReader.h"

#include "util.h"
#include "ast_builder.h"
#include "code_gen.h"
#include "cmd_parser.h"
#include "bf_error.h"
#include "lowering.h"


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

    // Lower the LLVM IR to it's specified target.
    lowering lower_pass(std::move(gen_pass.mod), std::move(gen_pass.machine));
    lower_pass.optimize(input.get_opt_level());

    // Determine what the object file should be.
    std::filesystem::path obj_file;

    if (input.option_exists("-o") && input.option_exists("-c")) obj_file = input.get_option("-o");
    else obj_file = std::string(std::filesystem::path(input.get_input_file()).stem()) + (input.option_exists("-S") ? ".s" : ".o");

    // Compile down the LLVM IR.
    lower_pass.compile(obj_file, input.option_exists("-S"));

    if (lower_pass.ec != brain_errc::no_err) {
        std::cerr << brain::err_msg(lower_pass.ec.message());
        return 1;
    } else if (input.option_exists("-S") || input.option_exists("-c")) {
        return 0;
    }

    // Get the executible name.
    std::filesystem::path exe_file;

    if (input.option_exists("-o")) exe_file = input.get_option("-o");
    else exe_file = std::filesystem::path(input.get_input_file()).stem();

    // Link the object file.
    lower_pass.link(obj_file, exe_file);

    // Since we lowered all the way to an executible, we now want to delete the object file.
    std::filesystem::remove(obj_file);

    if (lower_pass.ec != brain_errc::no_err) {
        std::cerr << brain::err_msg(lower_pass.ec.message());
        return 1;
    }

    return 0;
}