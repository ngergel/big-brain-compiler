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
#include <system_error>

#include "util.h"
#include "ast.h"
#include "bf_error.h"


class ast_builder {
public:

    // Error code for the AST generation.
    // If something goes wrong, the AST builder will silently fail and set this instead.
    std::error_code ec = brain_errc::no_err;
    std::shared_ptr<ast> err_node;

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

    // Check the syntax of the entire program, and set the error code if there is an issue.
    bool check_syntax();
};