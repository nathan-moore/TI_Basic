cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_COMPILER "/usr/local/bin/clang++")
set(CMAKE_C_COMPILER "/usr/local/bin/clang")

project(BASIC_Compiler)
project(Compiler_Tests)
project(Comp_library)

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})


add_compile_options(-Wall -Wextra -Werror -g)

find_package(FLEX)
find_package(BISON)

BISON_TARGET(MyParser src/Grammer.y ${CMAKE_CURRENT_BINARY_DIR}/Grammer.tab.cc
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/Grammer.tab.hh
             VERBOSE)

FLEX_TARGET(MyScanner src/Lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

add_library(Comp_library
    ${BISON_MyParser_OUTPUTS}
    ${FLEX_MyScanner_OUTPUTS}
    src/ASTNode.cpp
    src/ParsingHandler.cpp
    src/BasicBlockFormer.cpp
    src/IRGen.cpp)

add_executable(BASIC_Compiler
    src/test.cpp)

add_executable(Compiler_Tests
    test/src/main.cpp
    test/src/TestSetup.cpp
    test/src/ASTTests.cpp
    test/src/BBTests.cpp)

add_dependencies(BASIC_Compiler Comp_library)
add_dependencies(Compiler_Tests Comp_library)

target_link_libraries(BASIC_Compiler Comp_library)
target_link_libraries(Compiler_Tests Comp_library)

target_include_directories(Compiler_Tests PRIVATE test/include)
target_compile_features(Comp_library PUBLIC cxx_std_17)