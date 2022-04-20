// ------------------------------------------------------------
//  ast.h
//  
//  Definition of the AST class.
// ------------------------------------------------------------

#pragma once


// Include statements.
#include <memory>
#include <utility>
#include <vector>

#include "util.h"


struct ast : public std::enable_shared_from_this<ast> {

    // Declare the token and children, and parent of this AST node.
    brain::token token = brain::nil;
    std::vector<std::shared_ptr<ast> > children;
    std::shared_ptr<ast> parent = nullptr;

    // Line number and character number in it's line. Mainly used for error handling.
    // Also the index into the program string, when needed.
    size_t line = 0, chr = 0, idx = 0;

    // Constructors and deconstructors.
    ast() = default;
    ast(brain::token t): token(t) {};
    ast(brain::token t, std::shared_ptr<ast> p, size_t l, size_t c, size_t i):
        token(t), parent(p), line(l), chr(c), idx(i) {};

    ~ast() = default;

};
