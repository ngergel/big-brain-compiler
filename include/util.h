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
    const bool DEBUG = true;

    // Cell array size. Set to 65536 so 16-bit ints automatically wrap,
    // for bigger a array this wrapping needs to be implemented.
    const size_t CELL_SIZE = 65536;

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

    // Print out a tree in it's entirety, showing the tree structure.
    void print_ast(std::shared_ptr<ast>& t);

    // Print out the commandline usage, or help message.
    void print_usage();
    void print_help();
}