// ------------------------------------------------------------
//  ast.h
//  
//  Definition of the AST class.
//  
//  (c) Noah Gergel 2021
// ------------------------------------------------------------

#pragma once


// Include statements.
#include <memory>
#include <utility>
#include <vector>

#include "util.h"


struct ast : public std::enable_shared_from_this<ast> {

    // Declare the token and children, and parent of this AST node.
    brain::token token;
    std::vector<std::shared_ptr<ast> > children;
    std::shared_ptr<ast> parent;

    // Line number and character number in it's line. Mainly used for error handling.
    size_t line_num, char_num;

    // Constructors and deconstructors.
    ast() = default;
    ast(brain::token t): token(t) {};
    ast(brain::token t, std::shared_ptr<ast>& p): token(t), parent(p) {};

    ~ast() = default;

};
