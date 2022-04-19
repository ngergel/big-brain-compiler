// ------------------------------------------------------------
//  ast_builder.cpp
// 
//  Implementation of the AST builder pass.
// 
//  (c) Noah Gergel 2021
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
void ast_builder::visit(std::shared_ptr<ast>& t, std::string prog, bool is_root) {

    // If we are visiting the root of the program, go to the root visitor function.
    if (is_root) {
        visit_multi(t, brain::root, prog);
        return;
    }

    // Scan through the program text until we get to a valid token.
    size_t it = 0;
    while (it < prog.length() - 1 && !brain::valid_token(prog[it])) it++;

    // Reduce into each of the possible cases.
    // Note that we can't directly visit the root node,
    // that is artificially inserted when building the AST.
    switch (prog[it]) {
        case '+':
            visit_single(t, brain::plus); break;
        case '-':
            visit_single(t, brain::minus); break;
        case '.':
            visit_single(t, brain::period); break;
        case ',':
            visit_single(t, brain::comma); break;
        case '<':
            visit_single(t, brain::larrow); break;
        case '>':
            visit_single(t, brain::rarrow); break;
        case '[':
            visit_multi(t, brain::loop, prog.substr(1, prog.length() - 2)); break;
        default:
            visit_single(t, brain::nil); break;
    }
}


// ------------------------------------------------------------
//  visit_multi
// 
//  Visit a node that will potentially have children.
//  In bf, this only applies to the root or loop nodes.
// ------------------------------------------------------------
void ast_builder::visit_multi(std::shared_ptr<ast>& t, brain::token tok, std::string prog) {

    // Set the node's token.
    t->token = tok;

    // If this is a root node, set it's line/char position here.
    if (tok == brain::root) t->line_num = t->char_num = 1;

    // Scan through the program visiting valid tokens.
    size_t line_pos = t->line_num, char_pos = t->char_num;
    for (size_t i = 0; i < prog.length(); i++, char_pos++) {
        if (prog[i] == '\n') line_pos++, char_pos = 0;
        if (!brain::valid_token(prog[i])) continue;

        // Make a new AST node and walk the tree.
        std::shared_ptr<ast> child = std::make_shared<ast>();
        t->children.push_back(child), child->parent = t;
        child->line_num = line_pos, child->char_num = char_pos;

        // Make sure to handle loops seperately.
        visit(child, (prog[i] != '[' ? std::string{prog[i]} : prog.substr(i, loop_lookahead(prog, i) - i + 1)));
        
        // Update our index and line/char positions appropriately.
        if (prog[i] == '[') {
            size_t loop_size = loop_lookahead(prog, i) - i;
            for (size_t j = 0; j < loop_size; j++, char_pos++) {
                if (prog[i + j] == '\n') line_pos++, char_pos = 1;
            }

            i += loop_lookahead(prog, i) - i;
        }
    }
}


// ------------------------------------------------------------
//  visit_single
// 
//  Visit a simple or atomic node in the AST. These are most
//  of the instructions we will see, like + or -.
// ------------------------------------------------------------
void ast_builder::visit_single(std::shared_ptr<ast>& t, brain::token tok) {
    t->token = tok;
}


// ------------------------------------------------------------
//  loop_lookahead
// 
//  Scan through the program looking for the index of the
//  closing bracket, where the opening bracket is at start.
// ------------------------------------------------------------
size_t ast_builder::loop_lookahead(std::string prog, size_t start) {

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