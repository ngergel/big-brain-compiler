// ------------------------------------------------------------
//  cmd_parser.cpp
//  
//  Implementation of the command parser's methods.
// ------------------------------------------------------------


// Include statements.
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <filesystem>

#include "cmd_parser.h"
#include "util.h"
#include "bf_error.h"


// ------------------------------------------------------------
//  option_exists
// 
//  Checks if a given option exists.
// ------------------------------------------------------------
bool cmd_parser::option_exists(const std::string& opt) {

    // Retrieve an iterator to the given option using std::find.
    auto it = std::find(args.begin(), args.end(), opt);

    return it != args.end();
}


// ------------------------------------------------------------
//  get_option
//
//  Get the associated value with a given option, or return
//  the empty string.
// ------------------------------------------------------------
std::string cmd_parser::get_option(const std::string& opt) {

    // Retrieve an iterator to the given option using std::find.
    auto it = std::find(args.begin(), args.end(), opt);

    if (it == args.end()) return "";
    if (arg_flags.count(*it) || arg_optimization.count(*it)) return *it;
    
    return ++it != args.end() ? *it : "";
}


// ------------------------------------------------------------
//  get_input_file
//
//  Get the input file file.
// ------------------------------------------------------------
std::string cmd_parser::get_input_file() {

    // Loop through every argument.
    // We ignore arguments that have an option parameter preceeding them.
    for (size_t i = 1; i < args.size(); i++) {
        if (!arg_flags.count(args[i])
            && !arg_optimization.count(args[i])
            && !arg_parameters.count(args[i - 1])) {
            
            return args[i];
        }
    }

    // Otherwise, return an empty string.
    return "";
}


// ------------------------------------------------------------
//  check_input_file
// 
//  Checks that the input file exists and is a regular file.
// ------------------------------------------------------------
bool cmd_parser::check_input_file() {

    // Get the input file's status first.
    std::filesystem::file_status s = std::filesystem::status(get_input_file());

    // Make sure the input file is actually given.
    if (get_input_file() == "") {
        ec = brain_errc::cmd_missing_input;
    } else if (!std::filesystem::exists(s) || !std::filesystem::is_regular_file(s)) {
        ec = brain_errc::cmd_invalid_input;
    }

    return ec == brain_errc::no_err;
}


// ------------------------------------------------------------
//  get_opt_level
// 
//  Get the optimization level, as an integer from 0 to 3.
// ------------------------------------------------------------
size_t cmd_parser::get_opt_level() {

    // Check if any optimization levels are given.
    unsigned opt = 2;
    for (std::string o : arg_optimization) {
        if (option_exists(o)) opt = std::stoi(o.substr(2, 1));
    }

    return opt;
}