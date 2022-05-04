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
    if (opt_flags.count(*it) || opt_optimization.count(*it)) return *it;
    
    return ++it != args.end() ? *it : "";
}


// ------------------------------------------------------------
//  get_input_file
//
//  Get the input file file.
// ------------------------------------------------------------
std::filesystem::path cmd_parser::get_input_file() {

    // Loop through every argument, looking for the bf extension.
    // We ignore arguments that have an option parameter preceeding them.
    for (size_t i = 0; i < args.size(); i++) {
        std::filesystem::path file = args[i];
        if (file.extension() == ".bf" && (i == 0 || !opt_parameters.count(args[i - 1]))) return file;
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
        std::cerr << "\x1B[31mError:\033[0m Missing input file. Input files are expected to have the '.bf' extension.\n";
        return false;
    }

    // Make sure it exists.
    if (!std::filesystem::exists(s)) {
        std::cerr << "\x1B[31mError:\033[0m Input file does not exist.\n";
        return false;
    }

    // Make sure it is a regular file.
    if (!std::filesystem::is_regular_file(s)) {
        std::cerr << "\x1B[31mError:\033[0m Input is not a regular file.\n";
        return false;
    }

    return true;
}