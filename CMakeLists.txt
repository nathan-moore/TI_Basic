cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_C_COMPILER "clang")

project(BASIC_Compiler)
project(Compiler_Tests)
project(Comp_library)

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})


add_compile_options(-Wall -Werror -g)

find_package(FLEX)
find_package(BISON)
find_package(LLVM REQUIRED CONFIG)

BISON_TARGET(MyParser src/Grammer.y ${CMAKE_CURRENT_BINARY_DIR}/Grammer.tab.cc
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/Grammer.tab.hh
             VERBOSE)

FLEX_TARGET(MyScanner src/Lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(Comp_library
    ${BISON_MyParser_OUTPUTS}
    ${FLEX_MyScanner_OUTPUTS}
    src/ASTNode.cpp
    src/ParsingHandler.cpp
    src/BasicBlockFormer.cpp
    src/IRGen.cpp
    src/SymbolTable.cpp
    src/SSAFormer.cpp
    src/TypeBuilder.cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(BASIC_Compiler
    src/Compiler.cpp)

add_executable(Compiler_Tests
    test/src/main.cpp
    test/src/TestSetup.cpp
    test/src/ASTTests.cpp
    test/src/BBTests.cpp
    test/src/CodeGenTest.cpp
    test/src/TestCode.cpp)

add_dependencies(BASIC_Compiler Comp_library)
add_dependencies(Compiler_Tests Comp_library)

llvm_map_components_to_libnames(llvm_libs support core ExecutionEngine nativecodegen orcjit Interpreter MCJIT)

target_link_libraries(Comp_library ${llvm_libs})

target_link_libraries(BASIC_Compiler Comp_library)
target_link_libraries(Compiler_Tests Comp_library)

target_include_directories(Compiler_Tests PRIVATE test/include)
target_compile_features(Comp_library PUBLIC cxx_std_17)