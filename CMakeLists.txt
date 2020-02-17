cmake_minimum_required(VERSION 3.6)

project(BASIC_Compiler)

include_directories(include)

add_compile_options(-Wall -Wextra -Werror)

add_custom_command(OUTPUT src/Parser.tab.cc include/Parser.tab.hh
    COMMAND bison -t -d src/Parser.y
    COMMAND mv Parser.tab.cc src/Parser.tab.cc
    COMMAND mv Parser.tab.hh include/Parser.tab.hh
    DEPENDS src/Parser.y)

add_executable(BASIC_Compiler
    src/Parser.tab.cc
    src/test.cpp)