#include "IRGen.hpp"

void IRGen::FormIR(std::shared_ptr<BasicBlock>)
{
}

llvm::Value* IRGen::WalkNode(InstructionNode*)
{
	return nullptr;
}

llvm::Value* IRGen::WalkNode(FlowControl*, llvm::Value*, llvm::Value*, std::optional<llvm::Value*>)
{
	return nullptr;
}

llvm::Value* IRGen::WalkNode(BinaryExpNode*, llvm::Value*, llvm::Value*)
{
	return nullptr;
}

llvm::Value* IRGen::WalkNode(VariableNode*)
{
	return nullptr;
}

llvm::Value* IRGen::WalkNode(LiteralNode*)
{
	return nullptr;
}

llvm::Value* IRGen::WalkNode(LblNode*)
{
	return nullptr;
}

llvm::Value* IRGen::WalkNode(GotoNode*)
{
	return nullptr;
}

llvm::Value* IRGen::WalkNode(BasicJump*, llvm::Value*, llvm::Value*, std::optional<llvm::Value*>)
{
	return nullptr;
}

