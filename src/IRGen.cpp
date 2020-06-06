#include "IRGen.hpp"

#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"

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

llvm::Value* IRGen::WalkNode(BinaryExpNode* node, llvm::Value* lhs, llvm::Value* rhs)
{
	switch (node->token)
	{
	case Instructions::Add:
		if (node->GetExpType() == Type::Float)
		{
			return builder.CreateFAdd(lhs, rhs);
		}
		else
		{
			assert(node->GetExpType() == Type::Integer);
			return builder.CreateAdd(lhs, rhs);
		}
	case Instructions::Multiply:
		if (node->GetExpType() == Type::Float)
		{
			return builder.CreateMul(lhs, rhs);
		}
		else
		{
			assert(node->GetExpType() == Type::Float);
			return builder.CreateFMul(lhs, rhs);
		}
	case Instructions::Equals:
		if (node->GetExpType() == Type::Float)
		{
			return builder.CreateFCmpOEQ(lhs, rhs);
		}
		else
		{
			return builder.CreateICmpEq(lhs, rhs);
		}
	default:
		break;
	}

	//TODO: Assign
	assert(false);
	return nullptr;
}

llvm::Value* IRGen::WalkNode(VariableNode*)
{
	return nullptr;
}

llvm::Value* IRGen::WalkNode(LiteralNode* literal)
{
	assert(literal->GetExpType() != Type::Dynamic &&
		literal->GetExpType() != Type::Unknown);

	if (literal->GetExpType() == Type::Float)
	{
		llvm::Type* t = llvm::Type::getFloatTy(context);
		return llvm::ConstantFP::get(t, std::get<float>(literal->literal));
	}
	else if (literal->GetExpType() == Type::Integer)
	{
		llvm::Type* t = llvm::Type::getInt32Ty(context);
		return llvm::ConstantInt::get(t, std::get<int>(literal->literal));
	}

	assert(false);
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

