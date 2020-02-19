#pragma once

class InstructionNode;
class FlowControl;
class BinaryExpNode;

class ASTWalker {
public:
    virtual void WalkNode(InstructionNode*) = 0;
    virtual void WalkNode(FlowControl*) = 0;
    virtual void WalkNode(BinaryExpNode*) = 0;
};