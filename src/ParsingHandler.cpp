#include "Driver.hh"
#include "BasicBlockFormer.hpp"
#include "SSAFormer.hpp"

#include <cstdlib>
#include <iostream>
#include <cassert>

extern FILE* yyin;

namespace yy{

void parser::error(const std::string& str)
{
    std::cout << "Failure at " << str << std::endl;
    throw std::runtime_error("Failure at " + str + "\n");
}

}

driver::driver()
: result(-1)
{
}

int driver::parse(FILE* f)
{
    yyin = f;
    yy::parser parse(*this);
    //parse.set_debug_level(1);
    result = parse();

    fclose(f);
    return result;
}


int driver::parseString(const std::string& str)
{
    FILE* f = fmemopen((void*)str.c_str(), str.length(), "r");
    return parse(f);
}

void driver::Compile()
{
    assert(topNode != nullptr);

    BasicBlockFormer former;
    bbs = former.ParseBlocks(std::move(topNode));

    SSAFormer ssaFormer;
    ssaFormer.FormSSABlocks(bbs);
}

int driver::parse(const std::string& fileName)
{
    FILE* f = fopen(fileName.c_str(), "r");
    return parse(f);
}

