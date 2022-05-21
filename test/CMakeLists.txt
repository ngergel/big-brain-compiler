# Listing of all the tests for cmake.

# Hello world variants.
add_test(NAME hello COMMAND ${CMAKE_SOURCE_DIR}/test/test.sh hello)
add_test(NAME hello-hard COMMAND ${CMAKE_SOURCE_DIR}/test/test.sh hello-hard)
add_test(NAME hello-short COMMAND ${CMAKE_SOURCE_DIR}/test/test.sh hello-short)

# Calculates a cell's size, works for various interpreters/compilers.
add_test(NAME cell-size COMMAND ${CMAKE_SOURCE_DIR}/test/test.sh cell-size)

# Misc. tests.
add_test(NAME fibonacci COMMAND ${CMAKE_SOURCE_DIR}/test/test.sh fibonacci)
add_test(NAME give-you-up COMMAND ${CMAKE_SOURCE_DIR}/test/test.sh give-you-up)