#include <ASTNode.hpp>
#include <BasicBlock.hpp>
#include <NodeVisitor.hpp>
#include "BasicBlockFormer.hpp"

BasicBlockFormer::BasicBlockFormer()
{
}

void BasicBlockFormer::WalkNode(InstructionNode*)
{
}

void BasicBlockFormer::WalkNode(LblNode*)
{
}

void BasicBlockFormer::WalkNode(GotoNode*)
{
	//End of BB
}

void BasicBlockFormer::WalkNode(FlowControl* block)
{
	block->MakeBasicBlocks();

}

void BasicBlockFormer::WalkNode(BinaryExpNode*)
{
	return;
}

void BasicBlockFormer::WalkNode(VariableNode*)
{
	return;
}

void BasicBlockFormer::WalkNode(LiteralNode*)
{
	return;
}
