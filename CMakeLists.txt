# Minimum cmake version
cmake_minimum_required(VERSION 3.5)

# Use Boost
find_package(Boost)

# Name our project
project(Clang_Ast2Dot)

# Create executable target clang_ast2dot
add_executable(clang_ast2dot src/clang_ast2dot.cc)
target_compile_options(clang_ast2dot PUBLIC "-std=c++11")
target_link_libraries(clang_ast2dot "boost_regex;boost_program_options;boost_system")

# Create static library target gtestall
add_library(gtestall STATIC googletest/googletest/src/gtest-all.cc)
target_compile_options(gtestall PUBLIC "-std=c++11")
target_include_directories(gtestall PRIVATE googletest/googletest)
target_include_directories(gtestall SYSTEM BEFORE PRIVATE googletest/googletest/include)

# Create static executable target test_parser
add_executable(test_parser tests/test_parser.cc src/clang_ast_parser.cc)
target_compile_options(test_parser PUBLIC "-std=c++11")
target_include_directories(test_parser PRIVATE googletest/googletest)
target_include_directories(test_parser PRIVATE src)
target_include_directories(test_parser SYSTEM BEFORE PRIVATE googletest/googletest/include)
target_link_libraries(test_parser "gtestall;pthread")
