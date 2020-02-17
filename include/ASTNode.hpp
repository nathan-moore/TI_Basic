#pragma once

#include <string>
#include <initializer_list>
#include <vector>
#include <memory>

enum class Instructions
{
    IF,
    Disp
};

class AstNode
{
public:
    virtual ~AstNode() {}

};

//TODO: change?
class InstructionNode: public AstNode
{
public:
    InstructionNode(Instructions, std::string);
};

class InstructionList
{
    std::vector<std::shared_ptr<AstNode>> list;
public:
    InstructionList();

    void push_back(std::shared_ptr<AstNode>);
};

class ExpNode : public AstNode
{
private:
    std::vector<std::shared_ptr<AstNode>> nodes;
    Instructions token;
public:
    ExpNode(Instructions, std::initializer_list<std::shared_ptr<AstNode>>);
};

class FlowControl : public AstNode
{
public:
    //First Condition
    //Second First branch
    //Third Optional Else branch
    FlowControl(std::shared_ptr<ExpNode>, std::shared_ptr<InstructionList>, std::shared_ptr<InstructionList>);
};

