// ------------------------------------------------------------
//  util.h
//  
//  Tokens, constants and project wide settings.
//  
//  (c) Noah Gergel 2021
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

    // Print out a tree in it's entirety, showing the tree structure.
    void print_ast(std::shared_ptr<ast>& t);
}