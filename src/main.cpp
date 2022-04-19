// ------------------------------------------------------------
//  main.cpp
//  
//  Main file and entry point for the compiler.
// 
//  (c) Noah Gergel 2021
// ------------------------------------------------------------

// Include statments.
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "util.h"
#include "ast_builder.h"


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
    ast_builder ast_pass;
    std::shared_ptr<ast> tree = std::make_shared<ast>();
    ast_pass.visit(tree, bf_prog.str(), true);

    brain::print_ast(tree);

    return 0;
}