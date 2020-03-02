#include "AstTester.hpp"
#include "catch.hpp"
#include "Driver.hh"
#include <variant>

#include <stdio.h>

AstTestWalker::AstTestWalker(std::initializer_list<std::pair<Node, optionalData>> args)
    :toWalk(args),
    index(0)
{}

void AstTestWalker::RunTest(std::string input)
{
    index = 0;

    std::cout << "Parsing: " << input << std::endl;
    driver d;
    d.parseString(input);
    std::shared_ptr<InstructionList> topNode = d.topNode;
    topNode->InOrderWalk(this);

    REQUIRE(index == toWalk.size());
}

optionalData AstTestWalker::CheckNode(Node expected)
{
    auto [n, inst] = toWalk.at(index);
    index++;
    REQUIRE(n == expected);

    return inst;
}

void AstTestWalker::WalkNode(InstructionNode* node)
{
    using namespace std;

    auto data = CheckNode(Node::InstructionNode);

    REQUIRE(node->instruction == get<0>(data).value());
}

void AstTestWalker::WalkNode(FlowControl*)
{
    CheckNode(Node::FlowControl);
}

void AstTestWalker::WalkNode(BinaryExpNode*)
{
    CheckNode(Node::BinaryExpNode);
}

void AstTestWalker::WalkNode(VariableNode*)
{
    CheckNode(Node::VariableNode);
}

void AstTestWalker::WalkNode(LiteralNode*)
{
    CheckNode(Node::Literal);
}

void AstTestWalker::WalkNode(LblNode*)
{
    CheckNode(Node::LblNode);
}

void AstTestWalker::WalkNode(GotoNode*)
{
    CheckNode(Node::GotoNode);
}
