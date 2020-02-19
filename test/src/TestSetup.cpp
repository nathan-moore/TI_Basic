#include "AstTester.hpp"
#include "catch.hpp"
#include "Driver.hh"

#include <stdio.h>

AstTestWalker::AstTestWalker(std::initializer_list<std::pair<Node, std::variant<Instructions, std::string>>> args)
    :toWalk(args),
    index(0)
{}

void AstTestWalker::RunTest(std::string input)
{
    driver d;
    d.parseString(input);
    std::shared_ptr<InstructionList> topNode = d.topNode;
    topNode->InOrderWalk(this);

    REQUIRE(index == toWalk.size());
}

void AstTestWalker::WalkNode(InstructionNode*)
{
    auto n = toWalk.at(index).first;
    index++;
    REQUIRE( n == Node::InstructionNode );
}

void AstTestWalker::WalkNode(FlowControl*)
{
    auto n = toWalk.at(index).first;
    index++;
    REQUIRE( n == Node::InstructionNode );
}

void AstTestWalker::WalkNode(BinaryExpNode*)
{
    auto n = toWalk.at(index).first;
    index++;
    REQUIRE( n == Node::InstructionNode );
}