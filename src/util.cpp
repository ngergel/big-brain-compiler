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
//  print_prog
// 
//  Prints out the program, mainly for debugging purposes.
// ------------------------------------------------------------
void brain::print_prog(std::shared_ptr<ast>& t) {
    
    // Print this node's information.
    if (t->token == brain::loop) std::cout << "[";
    else if (t->token != brain::root) std::cout << token_name(t->token);

    // Print out the children nodes.
    for (auto child : t->children) print_ast(child);

    if (t->token == brain::loop) std::cout << "]";
    else if (t->token == brain::root) std::cout << "\n";
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
//  err_msg
// 
//  Get the formatted error message from a base message.
// ------------------------------------------------------------
std::string brain::err_msg(std::string msg, const std::shared_ptr<ast> src) {
    
    // Start with just the error word first.
    std::string err = "\x1B[31mError";

    // Add the line/char number if applicable.
    if (src != nullptr) err += "[" + std::to_string(src->line + 1) + ":" + std::to_string(src->chr + 1) + "]";

    // Add the rest of the message, and return.
    return err + ":\033[0m " + msg + "\n";
}