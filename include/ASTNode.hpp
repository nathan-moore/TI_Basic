#pragma once

#include <string>
#include <initializer_list>
#include <vector>
#include <memory>
#include <variant>
#include <queue>
#include <set>
#include <cassert>

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
    virtual void PreOrderWalk(ASTWalker* walker) = 0;
    virtual void InOrderWalk(ASTWalker* walker) = 0;
    virtual void PostOrderWalk(ASTWalker* walker) = 0;
    virtual Node GetType() const = 0;
};

class ExpNode : public AstNode
{ };

class Variable;
class SSAVariable;

class VariableNode : public ExpNode
{
    std::string name;
    bool isAssignment;
    Variable* var;
    SSAVariable* ssaVariable;
public:

    VariableNode(std::string& str) : name(str), isAssignment(false), var(nullptr), ssaVariable(nullptr)
    { }

    void PreOrderWalk(ASTWalker* walker) override;
    void InOrderWalk(ASTWalker* walker) override;
    void PostOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::VariableNode; }
    void SetAssignment() { isAssignment = true; }
    void SetVariable(Variable* variable) { var = variable; }
    Variable* getVariable() { return var; }
    void SetSSAVariable(SSAVariable* var) { ssaVariable = var; }
    const std::string& GetName() { return name; }
};

class LiteralNode : public ExpNode
{
public:
    const std::variant<int, float> literal;

    LiteralNode(int i) : literal(i) {}

    LiteralNode(float f) : literal(f) {}
    void PreOrderWalk(ASTWalker* walker) override;
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
    void PreOrderWalk(ASTWalker* walker) override;
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

    void push_front(std::shared_ptr<AstNode>);
    void push_back(std::shared_ptr<AstNode>);

    void PreOrderWalk(ASTWalker* walker);
    void InOrderWalk(ASTWalker*);
    void PostOrderWalk(ASTWalker* walker);

    using iterator = nodes_t::iterator;

    iterator begin() { return list.begin(); }
    iterator end() { return list.end(); }

    std::unique_ptr<InstructionList> MoveOffStart(std::shared_ptr<AstNode>& node)
    {
        auto newList = std::make_unique<InstructionList>();
        size_t i;
        for (i = 0; i < list.size(); i++)
        {
            newList->push_back(list[i]);
            if (list[i] == node)
            {
                break;
            }
        }

        list.erase(list.begin(), list.begin() + i + 1);
        return newList;
    }
};

class BinaryExpNode : public ExpNode
{
public:
    const std::shared_ptr<ExpNode> leftNode;
    const std::shared_ptr<ExpNode> rightNode;
    const Instructions token;

    BinaryExpNode(Instructions, std::shared_ptr<ExpNode>, std::shared_ptr<ExpNode>);
    void PreOrderWalk(ASTWalker* walker) override;
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

    std::unique_ptr<InstructionList> moveIfList()
    {
        return std::move(ifList);
    }

    std::unique_ptr<InstructionList> moveElseList()
    {
        return std::move(elseList);
    }

    //First Condition
    //Second First branch
    //Third Optional Else branch
    FlowControl(std::shared_ptr<ExpNode>, std::unique_ptr<InstructionList>, std::unique_ptr<InstructionList>);
    FlowControl(std::shared_ptr<ExpNode> n1, std::unique_ptr<InstructionList>&& n2)
        :FlowControl(n1, std::move(n2), nullptr) {}
    void PreOrderWalk(ASTWalker* walker) override;
    void InOrderWalk(ASTWalker* walker) override;
    void PostOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::FlowControl; }
};

class LblNode;

class GotoNode : public AstNode
{
    std::variant<std::string, std::shared_ptr<LblNode>> destination;

public:
    GotoNode(std::string block)
        : destination(block) {}

    GotoNode(std::string& block)
        : destination(block) {}

    GotoNode(std::shared_ptr<LblNode> lbl)
        : destination(lbl) {}

    // Inherited via AstNode
    void PreOrderWalk(ASTWalker* walker) override;
    virtual void InOrderWalk(ASTWalker* walker) override;
    virtual void PostOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::GotoNode; }

    bool HasBB();

    BasicBlock* GetBB();
    std::string& GetStr();

    void SetLabel(std::shared_ptr<LblNode> lbl)
    {
        assert(!HasBB());
        destination = lbl;
    }
};

class BasicJump : public AstNode
{
    std::shared_ptr<ExpNode> condition;
    std::shared_ptr<GotoNode> ifJmp;
    std::shared_ptr<GotoNode> elseJmp;
public:
    BasicJump(std::shared_ptr<ExpNode> cond, std::shared_ptr<GotoNode> ifNode, std::shared_ptr<GotoNode> elseNode)
        :condition(cond),
        ifJmp(ifNode),
        elseJmp(elseNode) {}

    void PreOrderWalk(ASTWalker* walker) override;
    void InOrderWalk(ASTWalker* walker) override;
    void PostOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::BasicJump; }

    std::shared_ptr<ExpNode>& condReference() { return condition; }
    BasicBlock* getIfBB() { return ifJmp->GetBB(); }
    BasicBlock* getElseBB() 
    {
        if (elseJmp != nullptr)
            return elseJmp->GetBB();
        else
            return nullptr;
    }
};

class LblNode : public AstNode
{
    std::string name;
    BasicBlock* containingBB;

public:
    LblNode(std::string str) : name(str), containingBB(nullptr) {}
    LblNode(std::string str, BasicBlock* bb) : name(str), containingBB(bb) {}

    // Inherited via AstNode
    void PreOrderWalk(ASTWalker* walker) override;
    virtual void InOrderWalk(ASTWalker* walker) override;
    Node GetType() const override { return Node::LblNode; }
    const std::string& GetLbl() const { return name; }
    BasicBlock* GetBB() const { return containingBB; }
    void SetBB(BasicBlock* bb)
    {
        assert(containingBB == nullptr);
        containingBB = bb;
    }

    // Inherited via AstNode
    virtual void PostOrderWalk(ASTWalker* walker) override;
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


class ASNodePrinter : public ASTWalker
{
    std::queue<BasicBlock*> toPrint;
    std::set<BasicBlock*> printed;
public:
    // Inherited via ASTWalker
    virtual void WalkNode(InstructionNode*) override;
    virtual void WalkNode(FlowControl*) override;
    virtual void WalkNode(BinaryExpNode*) override;
    virtual void WalkNode(VariableNode*) override;
    virtual void WalkNode(LiteralNode*) override;
    virtual void WalkNode(LblNode*) override;
    virtual void WalkNode(GotoNode*) override;
    virtual void WalkNode(BasicJump*) override;

    void WalkBBs(BasicBlock*);
};