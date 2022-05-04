// ------------------------------------------------------------
//  cmd_parser.h
//  
//  Class for parsing commandline options.
// ------------------------------------------------------------

#pragma once


// Include statements.
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_set>

#include "util.h"


class cmd_parser {
public:

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
    bool option_argument();

    // Get specifically the input bf file.
    std::filesystem::path get_input_file();

    // Check input file integrity.
    bool check_input_file();

private:

    // String consisting of the commandline arguments.
    std::vector<std::string> args;

    // Collection of valid option parameters, and flags.
    const std::unordered_set<std::string> opt_parameters{"-o"};
    const std::unordered_set<std::string> opt_flags{"-h"};
    const std::unordered_set<std::string> opt_optimization{"-O0", "-O1", "-O2", "-O3"};
};
