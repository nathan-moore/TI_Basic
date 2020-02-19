#include "ASTNode.hpp"

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
    std::shared_ptr<AstNode> left,
    std::shared_ptr<AstNode> right)
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

FlowControl::FlowControl(std::shared_ptr<BinaryExpNode> exp, 
    std::shared_ptr<InstructionList> ifBranch,
    std::shared_ptr<InstructionList> elseBranch)
    : condition(exp),
    ifList(ifBranch),
    elseList(elseBranch)
{}

void FlowControl::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
    condition->InOrderWalk(walker);
    ifList->InOrderWalk(walker);
    if (elseList != nullptr)
    {
        elseList->InOrderWalk(walker);
    }
}