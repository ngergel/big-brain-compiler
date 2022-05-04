// ------------------------------------------------------------
//  ast_builder.h
//  
//  This pass constructs the AST from the input program.
// ------------------------------------------------------------

#pragma once


// Include statements.
#include <memory>
#include <utility>
#include <vector>
#include <string>

#include "util.h"
#include "ast.h"


class ast_builder {
public:

    // Constructors and deconstructors.
    ast_builder() = default;
    ast_builder(std::string p): prog(p) {};

    ~ast_builder() = default;

    // Main visitor function.
    void visit(std::shared_ptr<ast>& t);

private:

    // Copy of the program to be referenced when constructing the AST.
    std::string prog;

    // Visitor functions for each type of token.
    // Mainly, is it a single token or does it have children.
    void visit_multi(std::shared_ptr<ast>& t);
    void visit_single(std::shared_ptr<ast>& t);

    // Helper function for getting the index of the closing bracket in a loop.
    size_t loop_lookahead(size_t start);

    // Helper function for incrementing the index, respecting char and line position.
    void incr_idx(size_t& idx, size_t& line, size_t& chr);
};