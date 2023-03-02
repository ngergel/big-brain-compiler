// ------------------------------------------------------------
//  util.h
//  
//  Tokens, constants and project wide settings.
//  General utility functions.
// ------------------------------------------------------------

#pragma once


// Include statements.
#include <memory>
#include <utility>
#include <string>


struct ast;


namespace brain {

    // Debug flag.
    const bool DEBUG = false;

    // Cell array size. Set to 65536 so 16-bit ints automatically wrap,
    // for a bigger array this wrapping needs to be implemented.
    const size_t CELL_SIZE = 65536;

    // Usage string.
    const std::string USAGE = "\x1B[33mUsage:\033[0m brainc [-hv] [-O<n>] <input file> [-o <output file>]\n";

    // Options info string.
    const std::string OPTIONS = "Options:\n"
                                "  -h, --help           Display this information.\n"
                                "  -o <output file>     Specify the name of the output file.\n"
                                "  -O<n>                Set the optimization level. -O2 is default.\n"
                                "  -v, --version        Display the version information.\n";

    const std::string VERSION = "Big Brain Compiler (c) 2023 Noah Gergel\n"
                                "v1.1\n";

    // Define all of the important tokens in a bf program.
    enum token {
        root,
        plus,
        minus,
        loop,
        period,
        comma,
        larrow,
        rarrow,
        nil
    };

    // Given a character, checks if it is a valid token.
    bool valid_token(char c);

    // Given a token, return it's name.
    std::string token_name(token tok);

    // Given a character, return the appropriate token.
    token get_token(char c);

    // Print out the bf program, for debugging purposes.
    void print_prog(std::shared_ptr<ast>& t);

    // Print out a given error message.
    std::string err_msg(std::string msg, const std::shared_ptr<ast> src = nullptr);
}