# Gather our source files in this directory.
set(src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/util.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ast_builder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/code_gen.cpp")

# Add the executable.
add_executable(brainc ${src_files})

# Link against LLVM libraries.
target_link_libraries(brainc ${libs} ${sys_libs} ${ld_flags} ${cxx_flags})

# Move the executable to a project bin directory.
set_target_properties(brainc PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")