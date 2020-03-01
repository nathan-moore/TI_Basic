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
    Equals,
    Nothing,
    Add,
    Multiply
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
    void InOrderWalk(ASTWalker* walker) override;
};

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
    const std::shared_ptr<ExpNode> leftNode;
    const std::shared_ptr<ExpNode> rightNode;
    const Instructions token;

    BinaryExpNode(Instructions, std::shared_ptr<ExpNode>, std::shared_ptr<ExpNode>);
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

class BinaryExpNodeBuilder
{
    std::shared_ptr<ExpNode> left;
    Instructions token;

public:
    BinaryExpNodeBuilder()
        : left(nullptr),
        token(Instructions::Nothing)
    {}

    BinaryExpNodeBuilder(Instructions inst, std::shared_ptr<ExpNode> node)
        : left(node),
        token(inst) {}

    std::shared_ptr<BinaryExpNode> Build(std::shared_ptr<ExpNode> node)
    {
        return std::make_shared<BinaryExpNode>(token, left, node);
    }

    bool IsDefault()
    {
        return left == nullptr;
    }
};