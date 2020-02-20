#pragma once

#include <string>
#include <initializer_list>
#include <vector>
#include <memory>
#include <variant>

#include "NodeVisitor.hpp"

enum class Instructions
{
    Disp,
    Nothing,
    Equals
};

class AstNode
{
public:
    virtual ~AstNode() {}
    virtual void InOrderWalk(ASTWalker* walker) = 0;
};

class ExpNode : public AstNode
{ };

class VariableNode : public ExpNode
{
    //TODO: Symbol table?
};

//TODO: shouldn't inherit from ExpNode?
class LiteralNode : public ExpNode
{
public:
    const std::variant<int, float> literal;

    LiteralNode(int i) : literal(i) {}

    LiteralNode(float f) : literal(f) {}
    void InOrderWalk(ASTWalker* walker) override;
};

//TODO: change?
class InstructionNode: public AstNode
{
public:
    const std::variant<std::string, std::shared_ptr<VariableNode>> items;
    const Instructions instruction;

    InstructionNode(Instructions, std::string);

    void InOrderWalk(ASTWalker* walker) override;
};

class InstructionList
{
    std::vector<std::shared_ptr<AstNode>> list;
public:
    InstructionList();

    void push_back(std::shared_ptr<AstNode>);

    void InOrderWalk(ASTWalker*);
};

class BinaryExpNode : public ExpNode
{
public:
    const std::shared_ptr<AstNode> leftNode;
    const std::shared_ptr<AstNode> rightNode;
    const Instructions token;

    BinaryExpNode(Instructions, std::shared_ptr<AstNode>, std::shared_ptr<AstNode>);
    void InOrderWalk(ASTWalker* walker) override;
};

class FlowControl : public AstNode
{
public:
    const std::shared_ptr<ExpNode> condition;
    const std::shared_ptr<InstructionList> ifList;
    const std::shared_ptr<InstructionList> elseList;

    //First Condition
    //Second First branch
    //Third Optional Else branch
    FlowControl(std::shared_ptr<ExpNode>, std::shared_ptr<InstructionList>, std::shared_ptr<InstructionList>);
    void InOrderWalk(ASTWalker* walker) override;
};