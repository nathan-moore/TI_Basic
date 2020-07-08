#pragma once

class InstructionNode;
class FlowControl;
class BinaryExpNode;
class VariableNode;
class LiteralNode;
class LblNode;
class GotoNode;
class BasicJump;
class InstructionList;

class ASTWalker {
public:
    virtual void WalkNode(InstructionNode*) = 0;
    virtual void WalkNode(FlowControl*) = 0;
    virtual void WalkNode(BinaryExpNode*) = 0;
    virtual void WalkNode(VariableNode*) = 0;
    virtual void WalkNode(LiteralNode*) = 0;
    virtual void WalkNode(LblNode*) = 0;
    virtual void WalkNode(GotoNode*) = 0;
    virtual void WalkNode(BasicJump*) = 0;

    virtual void WalkList(InstructionList*)
    {
        return;
    }

    virtual void TopLevelNode()
    {
        return;
    }
};