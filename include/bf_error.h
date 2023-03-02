// ------------------------------------------------------------
//  bf_error.h
//  
//  Custom std::error_code overloading for the compiler.
// ------------------------------------------------------------

#pragma once


// Include statements.
#include <string>
#include <system_error>

#include "util.h"


// BF specific error codes.
enum class brain_errc {
    no_err = 0,
    cmd_missing_input,
    cmd_invalid_input,
    ast_lbracket,
    ast_rbracket,
    gen_bad_init,
    lower_output,
    lower_object,
    lower_clang,
    lower_linking,
    unknown
};

namespace std {
    template<>
    struct is_error_code_enum<brain_errc> : public true_type {};
}


// Category for the bf error codes. Overloads the appropriate std library methods.
namespace brain {
    class bf_category : public std::error_category {
    public:
        
        // Name of this category, to distinguish from std error categories.
        virtual const char* name() const noexcept {
            return "brainf";
        }

        // Error messages for every possible error code.
        virtual std::string message(int ev) const {
            
            switch (static_cast<brain_errc>(ev)) {
                case brain_errc::no_err:
                    return "no error";
                case brain_errc::cmd_missing_input:
                    return "missing input file";
                case brain_errc::cmd_invalid_input:
                    return "invalid input file";
                case brain_errc::ast_lbracket:
                    return "'[' is missing it's closing ']'";
                case brain_errc::ast_rbracket:
                    return "']' is missing it's opening '['";
                case brain_errc::gen_bad_init:
                    return "unable to initialize LLVM module";
                case brain_errc::lower_output:
                    return "could not open output file";
                case brain_errc::lower_object:
                    return "could not compile LLVM IR";
                case brain_errc::lower_clang:
                    return "could not find clang for linking";
                case brain_errc::lower_linking:
                    return "unable to link the object file with clang";
                default:
                    return "unknown error";
            }
        }
    };
}

extern inline const brain::bf_category &bf_category() {
  static brain::bf_category c;
  return c;
}

// Overload make_error_code for making bf error codes.
inline std::error_code make_error_code(brain_errc e) {
    return {static_cast<int>(e), bf_category()};
}