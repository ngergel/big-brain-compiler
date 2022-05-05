// ------------------------------------------------------------
//  util.cpp
//  
//  General project-wide utility functions.
// ------------------------------------------------------------


// Include statements.
#include <cassert>
#include <iostream>

#include "util.h"
#include "ast.h"


// ------------------------------------------------------------
//  valid_token
// 
//  Checks if the given char is a bf token.
// ------------------------------------------------------------
bool brain::valid_token(char c) {

    // We just go through all of the various cases and return
    // the appropriate tokens.
    switch (c) {
        case '+':
        case '-':
        case '[':
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
              << "Line and char position: " << t->line << ", " << t->chr << "\n";
    if (t->parent) std::cout << "Parent token: " << token_name(t->parent->token) << "\n";

    // Print out the children nodes.
    std::cout << "Children:\n";
    for (auto child : t->children) std::cout << token_name(child->token) << " ";
    std::cout << "\n\n";

    for (auto child : t->children) {
        if (child->token == brain::loop) print_ast(child);
    }
}


// ------------------------------------------------------------
//  token_name
// 
//  Given a token, return the name of the token as string.
// ------------------------------------------------------------
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
}


// ------------------------------------------------------------
//  get_token
// 
//  Given a char, return the corresponding token or nil.
// ------------------------------------------------------------
brain::token brain::get_token(char c) {

    // Just map the char to the right token.
    switch (c) {
        case '+':
            return brain::plus;
        case '-':
            return brain::minus;
        case '.':
            return brain::period;
        case ',':
            return brain::comma;
        case '<':
            return brain::larrow;
        case '>':
            return brain::rarrow;
        case '[':
            return brain::loop;
        default:
            return brain::nil;
    }
}


// ------------------------------------------------------------
//  get_err
// 
//  Get the formatted error message from a base message.
// ------------------------------------------------------------
std::string brain::get_err(std::string msg, const std::shared_ptr<ast> src) {
    
    // Start with just the error word first.
    std::string err = "\x1B[31mError";

    // Add the line/char number if applicable.
    if (src != nullptr) err += "[" + std::to_string(src->line + 1) + ":" + std::to_string(src->chr + 1) + "]";

    // Add the rest of the message, and return.
    return err + ":\033[0m " + msg + "\n";
}