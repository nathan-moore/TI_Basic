#pragma once

class InstructionNode;
class FlowControl;
class BinaryExpNode;
class VariableNode;
class LiteralNode;

class ASTWalker {
public:
    virtual void WalkNode(InstructionNode*) = 0;
    virtual void WalkNode(FlowControl*) = 0;
    virtual void WalkNode(BinaryExpNode*) = 0;
    virtual void WalkNode(VariableNode*) = 0;
    virtual void WalkNode(LiteralNode*) = 0;
};