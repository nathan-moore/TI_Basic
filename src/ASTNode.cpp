#include "ASTNode.hpp"
#include "BasicBlock.hpp"

#include <assert.h>

InstructionNode::InstructionNode(Instructions i, std::string s)
    :     items(s), 
    instruction(i)
{}

void InstructionNode::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

InstructionList::InstructionList() {}

void InstructionList::InOrderWalk(ASTWalker* walker)
{
    for (std::shared_ptr<AstNode> node : list)
    {
        node->InOrderWalk(walker);
    }
}

void InstructionList::push_back(std::shared_ptr<AstNode> node)
{
    list.push_back(node);
}

BinaryExpNode::BinaryExpNode(Instructions i, 
    std::shared_ptr<ExpNode> left,
    std::shared_ptr<ExpNode> right)
    : leftNode(left),
    rightNode(right),
    token(i)
{}

void BinaryExpNode::InOrderWalk(ASTWalker* walker)
{
    leftNode->InOrderWalk(walker);
    walker->WalkNode(this);
    rightNode->InOrderWalk(walker);
}

std::shared_ptr<ExpNode>& FlowControl::getCond()
{
    return condition;
}

std::unique_ptr<InstructionList>& FlowControl::getIfList()
{
    if (BBFormed)
        return ifList;
    else
        return IfBB->getInstructions();
}

std::unique_ptr<InstructionList>& FlowControl::getElseList()
{
    if (BBFormed)
        return elseList;
    else
        return ElseBB->getInstructions();
}

void FlowControl::MakeBasicBlocks()
{
    IfBB = std::make_shared<BasicBlock>(std::move(ifList));
    if (elseList != nullptr)
        ElseBB = std::make_shared<BasicBlock>(std::move(elseList));
}

std::shared_ptr<BasicBlock> FlowControl::getIfBB()
{
    assert(BBFormed);
    return IfBB;
}

std::shared_ptr<BasicBlock> FlowControl::getElseBB()
{
    assert(BBFormed);
    return ElseBB;
}

FlowControl::FlowControl(std::shared_ptr<ExpNode> exp,
    std::unique_ptr<InstructionList> ifBranch,
    std::unique_ptr<InstructionList> elseBranch)
    : condition(exp),
    ifList(std::move(ifBranch)),
    elseList(std::move(elseBranch))
{}

void FlowControl::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
    condition->InOrderWalk(walker);
    getIfList()->InOrderWalk(walker);
    std::unique_ptr<InstructionList>& elses = getElseList();
    if (elses != nullptr)
    {
        elses->InOrderWalk(walker);
    }
}

void LiteralNode::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void VariableNode::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void LblNode::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void GotoNode::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}
