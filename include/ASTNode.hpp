#pragma once

#include <string>
#include <initializer_list>
#include <vector>
#include <memory>
#include <variant>

#include "NodeVisitor.hpp"

enum class Instructions
{
    //Statement Instructions
    Disp,

    //Binary Instructions
    Equals,
    Nothing,
    Add,
    Multiply,
    Assign
};

enum class Node
{
    VariableNode,
    LiteralNode,
    InstructionNode,
    BinaryExpNode,
    FlowControl,
    LblNode,
    GotoNode, 
    BasicJump
};

class AstNode
{
public:
    virtual ~AstNode() {}
    virtual void InOrderWalk(ASTWalker* walker) = 0;
    virtual void PostOrderWalk(ASTWalker* walker) = 0;
    virtual Node GetType() const = 0;
};

class ExpNode : public AstNode
{ };

class VariableNode : public ExpNode
{
    //TODO: Symbol table?
    void InOrderWalk(ASTWalker* walker) override;
    void PostOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::VariableNode; }
};

class LiteralNode : public ExpNode
{
public:
    const std::variant<int, float> literal;

    LiteralNode(int i) : literal(i) {}

    LiteralNode(float f) : literal(f) {}
    void InOrderWalk(ASTWalker* walker) override;
    void PostOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::LiteralNode; }
};

//TODO: change?
class InstructionNode: public AstNode
{
public:
    const std::variant<std::string> items;
    const Instructions instruction;

    InstructionNode(Instructions, std::string);
    void InOrderWalk(ASTWalker* walker) override;
    void PostOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::InstructionNode; }
};

class InstructionList
{
    using nodes_t = std::vector<std::shared_ptr<AstNode>>;

    std::vector<std::shared_ptr<AstNode>> list;
public:
    InstructionList();

    void push_back(std::shared_ptr<AstNode>);

    void InOrderWalk(ASTWalker*);
    void PostOrderWalk(ASTWalker* walker);

    using iterator = nodes_t::iterator;

    iterator begin() { return list.begin(); }
    iterator end() { return list.end(); }
};

class BinaryExpNode : public ExpNode
{
public:
    const std::shared_ptr<ExpNode> leftNode;
    const std::shared_ptr<ExpNode> rightNode;
    const Instructions token;

    BinaryExpNode(Instructions, std::shared_ptr<ExpNode>, std::shared_ptr<ExpNode>);
    void InOrderWalk(ASTWalker* walker) override;
    void PostOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::InstructionNode; }
};

class BasicBlock;

class FlowControl : public AstNode
{
    std::shared_ptr<ExpNode> condition;
    std::unique_ptr<InstructionList> ifList;
    std::unique_ptr<InstructionList> elseList;

public:
    std::shared_ptr<ExpNode>& getCond();
    std::unique_ptr<InstructionList>& getIfList();
    std::unique_ptr<InstructionList>& getElseList();

    //First Condition
    //Second First branch
    //Third Optional Else branch
    FlowControl(std::shared_ptr<ExpNode>, std::unique_ptr<InstructionList>, std::unique_ptr<InstructionList>);
    FlowControl(std::shared_ptr<ExpNode> n1, std::unique_ptr<InstructionList>&& n2)
        :FlowControl(n1, std::move(n2), std::unique_ptr<InstructionList>{}) {}
    void InOrderWalk(ASTWalker* walker) override;
    void PostOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::FlowControl; }
};

class BasicJump : public AstNode
{
    std::shared_ptr<ExpNode> condition;
    std::shared_ptr<GotoNode> ifJmp;
    std::shared_ptr<GotoNode> elseJmp;
public:
    BasicJump(std::shared_ptr<ExpNode>, std::shared_ptr<GotoNode>, std::shared_ptr<GotoNode>);
    void InOrderWalk(ASTWalker* walker) override;
    void PostOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::BasicJump; }
};

class LblNode : public AstNode
{
    std::string name;

public:
    // Inherited via AstNode
    virtual void InOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::LblNode; }
    const std::string& GetLbl() const { return name; }
};

class GotoNode : public AstNode
{
    std::variant<std::string, std::shared_ptr<BasicBlock>> destination;

public:
    // Inherited via AstNode
    virtual void InOrderWalk(ASTWalker* walker) override;
    virtual void PostOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::GotoNode; }
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