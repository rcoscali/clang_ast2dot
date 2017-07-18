set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(Boost)

cmake_minimum_required(VERSION 3.5)

project(Clang_Ast2Dot)

add_executable(clang_ast2dot src/clang_ast2dot.cc)

TARGET_LINK_LIBRARIES(clang_ast2dot "boost_program_options;boost_system")

