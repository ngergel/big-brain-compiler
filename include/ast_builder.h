// ------------------------------------------------------------
//  ast_builder.h
//  
//  This pass constructs the AST from the input program.
//  
//  (c) Noah Gergel 2021
// ------------------------------------------------------------

#pragma once


// Include statements.
#include <memory>
#include <utility>
#include <vector>

#include "util.h"
#include "ast.h"


class ast_builder {
public:

    // Constructors and deconstructors.
    ast_builder() = default;
    ~ast_builder() = default;

    // Visitor methods for each type of token.
    void visit(std::shared_ptr<ast>& t, std::string prog, bool is_root = false);

    void visit_multi(std::shared_ptr<ast>& t, brain::token tok, std::string prog);
    void visit_single(std::shared_ptr<ast>& t, brain::token tok);

    // Helper function for getting the index of the closing bracket in a loop.
    size_t loop_lookahead(std::string prog, size_t start);

};