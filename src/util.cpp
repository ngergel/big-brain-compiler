// ------------------------------------------------------------
//  util.cpp
//  
//  General project-wide utility functions.
// 
//  (c) Noah Gergel 2021
// ------------------------------------------------------------


// Include statements.
#include <cassert>
#include <iostream>

#include "util.h"
#include "ast.h"


// ------------------------------------------------------------
//  valid_token
// 
//  Checks if the given token is a bf token.
// ------------------------------------------------------------
bool brain::valid_token(char c) {

    // We just go through all of the various cases and return
    // the appropriate tokens.
    switch (c) {
        case '+':
        case '-':
        case '[':
        case ']':
        case '.':
        case ',':
        case '<':
        case '>':
            return true;
        default:
            return false;
    }
}


// ------------------------------------------------------------
//  print_ast
// 
//  Prints out an AST, mainly for debugging purposes.
// ------------------------------------------------------------
void brain::print_ast(std::shared_ptr<ast>& t) {
    
    // Print this node's information.
    std::cout << "Token: " << token_name(t->token) << "\n"
              << "Number of children: " << t->children.size() << "\n"
              << "Line and char position: " << t->line_num << ", " << t->char_num << "\n";
    if (t->parent) std::cout << "Parent token: " << token_name(t->parent->token) << "\n";

    // Print out the children nodes.
    std::cout << "Children:\n";
    for (auto child : t->children) std::cout << token_name(child->token) << " ";
    std::cout << "\n\n";

    for (auto child : t->children) {
        if (child->token == brain::loop) print_ast(child);
    }
}

std::string brain::token_name(brain::token tok) {
    
    // Just map the token to it's appropriate name.
    switch (tok) {
        case brain::plus:
            return "+";
        case brain::minus:
            return "-";
        case brain::period:
            return ".";
        case brain::comma:
            return ",";
        case brain::larrow:
            return "<";
        case brain::rarrow:
            return ">";
        case brain::loop:
            return "loop";
        case brain::root:
            return "root";
        default:
            return "nil";
    }

    return "nil";
}