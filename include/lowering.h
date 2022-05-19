// ------------------------------------------------------------
//  lowering.h
//  
//  Pass for lowering the LLVM IR to executable/asm/object file.
// ------------------------------------------------------------

#pragma once


// Include statements.
#include <memory>
#include <utility>
#include <string>
#include <system_error>

#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"

#include "bf_error.h"


class lowering {
public:

    // Error code for the lowering pass.
    std::error_code ec = brain_errc::no_err;

    // The LLVM module/target machine to reference when lowering.
    std::unique_ptr<llvm::Module> mod;
    std::unique_ptr<llvm::TargetMachine> machine;

    // Constructors and deconstructors.
    lowering() = default;
    lowering(std::unique_ptr<llvm::Module> m, std::unique_ptr<llvm::TargetMachine> tm): mod(std::move(m)), machine(std::move(tm)) {};

    ~lowering() = default;

    // Optimize the LLVM IR to a given optimization level.
    void optimize(unsigned opt_level);

    // Compile and link respectively. Linking just involkes clang.
    void compile(std::string output_file, bool target_asm = false);
    void link(std::string obj_file, std::string exe_file);
private:

};