#pragma once

#include "ASTNode.hpp"
#include "NodeVisitor.hpp"

class BasicBlockFormer : public ASTWalker {
public:
	BasicBlockFormer();

	std::shared_ptr<BasicBlock> ParseBlocks(std::unique_ptr<InstructionList>);

	// Inherited via ASTWalker
	virtual void WalkNode(FlowControl*) override;
	virtual void WalkNode(BinaryExpNode*) override;
	virtual void WalkNode(VariableNode*) override;
	virtual void WalkNode(LiteralNode*) override;
	virtual void WalkNode(InstructionNode*) override;
	virtual void WalkNode(LblNode*) override;
	virtual void WalkNode(GotoNode*) override;
};