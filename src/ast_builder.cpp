// ------------------------------------------------------------
//  ast_builder.cpp
// 
//  Implementation of the AST builder pass.
// ------------------------------------------------------------

// Include statements.
#include <cassert>
#include <iostream>

#include "util.h"
#include "ast_builder.h"


// ------------------------------------------------------------
//  visit
// 
//  General visit function for parsing the AST.
// ------------------------------------------------------------
void ast_builder::visit(std::shared_ptr<ast>& t) {

    // Make sure we aren't visiting a nil token in the AST.
    if (brain::DEBUG) assert(t->token != brain::nil);

    // If the error code was set, just exit early.
    if (ec) return;

    // Reduce into each of the possible cases.
    switch (t->token) {
        case brain::plus:
        case brain::minus:
        case brain::period:
        case brain::comma:
        case brain::larrow:
        case brain::rarrow:
            visit_single(t);
            break;
        case brain::root:
        case brain::loop:
            visit_multi(t);
            break;
        default:
            t = std::make_shared<ast>(brain::nil);
    }
}


// ------------------------------------------------------------
//  visit_multi
// 
//  Visit a node that will potentially have children.
//  In bf, this only applies to the root or loop nodes.
// ------------------------------------------------------------
void ast_builder::visit_multi(std::shared_ptr<ast>& t) {

    // Once at the beginning, do a syntax check.
    if (t->token == brain::root && !check_syntax()) return;

    // Get the end of the program that we will look at.
    size_t end = t->token == brain::root ? prog.length() : loop_lookahead(t->idx);

    // Initialize the line, char, and index numbers.
    size_t line = t->line, chr = t->chr, i = t->idx;
    if (t->token != brain::root) incr_idx(i, line, chr);

    // Scan through the program visiting valid tokens.
    while (i < end) {

        // Make a new AST node and walk the tree if we have a valid token.
        if (brain::valid_token(prog[i])) {
            std::shared_ptr<ast> child = std::make_shared<ast>(brain::get_token(prog[i]), t, line, chr, i);
            t->children.push_back(child);
            visit(child);

            // Also move the index if the child's a loop, so we don't copy it twice.
            if (child->token == brain::loop) {
                size_t loop_end = loop_lookahead(i);
                while (i < loop_end) incr_idx(i, line, chr);
            }
        }

        incr_idx(i, line, chr);
    }
}


// ------------------------------------------------------------
//  visit_single
// 
//  Visit a simple or atomic node in the AST. These are most
//  of the instructions we will see, like + or -.
// ------------------------------------------------------------
void ast_builder::visit_single(std::shared_ptr<ast>& t) {}


// ------------------------------------------------------------
//  loop_lookahead
// 
//  Scan through the program looking for the index of the
//  closing bracket, where the opening bracket is at start.
// ------------------------------------------------------------
size_t ast_builder::loop_lookahead(size_t start) {

    // Assert that start is at the right token.
    if (brain::DEBUG) assert(prog.length() > start && prog[start] == '[');

    // We can nest loops, use cnt to find the matching right bracket.
    size_t cnt = 0, end = 0;

    // Loop through each character until we find the closing bracket.
    for (size_t i = start + 1; i < prog.length() && !end; i++) {
        if (prog[i] == '[') cnt++;
        else if (prog[i] == ']') cnt ? cnt-- : end = i;
    }

    // Return the index of the closing right bracket.
    return end;
}


// ------------------------------------------------------------
//  incr_idx
// 
//  Increment the index while respecting line/chr position.
//  Does the incrementing inplace.
// ------------------------------------------------------------
void ast_builder::incr_idx(size_t& idx, size_t& line, size_t& chr) {
    if (prog[idx] == '\n') idx++, line++, chr = 0;
    else idx++, chr++;
}


// ------------------------------------------------------------
//  check_syntax
// 
//  Check the program syntax. The only possible syntax error
//  are unbalanced brackets.
// ------------------------------------------------------------
bool ast_builder::check_syntax() {

    // Initialize a counter and loop through the program.
    int cnt = 0;
    size_t line = 0, chr = 0, first_line = 0, first_chr = 0;
    for (size_t i = 0; i < prog.length() && !ec; incr_idx(i, line, chr)) {
        if (prog[i] == '[') cnt++;
        else if (prog[i] == ']') cnt--;

        // We want the line/char of the first '[' for missing closing bracket errors.
        if (prog[i] == '[' && cnt == 1) first_line = line, first_chr = chr;
        if (cnt < 0) break;
    }

    // Print out the appropriate error message, or return true.
    if (cnt > 0 && (ec = 1)) {
        std::cerr << brain::get_err("'[' is missing it's closing ']'.",
                                    std::make_shared<ast>(brain::nil, nullptr, first_line, first_chr, 0));
        return false;
    } else if (cnt < 0 && (ec = 2)) {
        std::cerr << brain::get_err("']' is missing it's opening '['.",
                                    std::make_shared<ast>(brain::nil, nullptr, line, chr, 0));
        return false;
    }

    return true;
}