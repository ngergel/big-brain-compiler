// ------------------------------------------------------------
//  cmd_parser.h
//  
//  Class for parsing commandline options.
// ------------------------------------------------------------

#pragma once


// Include statements.
#include <vector>
#include <string>
#include <unordered_set>
#include <system_error>

#include "util.h"
#include "bf_error.h"


class cmd_parser {
public:

    // Error code for command parsing.
    std::error_code ec = brain_errc::no_err;

    // Constructor and deconstructor.
    cmd_parser() = default;
    cmd_parser(unsigned argc, char** argv) {
        
        // Loop through each argument, adding it to the vector.
        for (size_t i = 0; i < argc; i++) args.push_back(std::string{argv[i]});
    }

    ~cmd_parser() = default;

    // Functions for checking if an option exists, or returning it's associated value.
    bool option_exists(const std::string& opt);
    std::string get_option(const std::string& opt);

    // Get specifically the input bf file or optimization level.
    std::string get_input_file();
    size_t get_opt_level();

    // Check input file integrity.
    bool check_input_file();

private:

    // String consisting of the commandline arguments.
    std::vector<std::string> args;

    // Collection of valid option parameters, and flags.
    const std::unordered_set<std::string> arg_parameters{"-o"};
    const std::unordered_set<std::string> arg_flags{"-h", "--help", "help", "-v", "--version", "-c", "-S"};
    const std::unordered_set<std::string> arg_optimization{"-O0", "-O1", "-O2", "-O3"};
};
