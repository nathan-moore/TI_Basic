#include "TypeBuilder.hpp"
#include "SSAFormer.hpp"

void TypeBuilder::WalkNode(std::unique_ptr<InstructionList>& node, bool handleVarNodes)
{
	this->handleVarNodes = handleVarNodes;
	IteratedNewInfo = false;

	PostOrderWalkerHelper<TypeBuilder*, Type> helper(this);
	node->PostOrderWalk(&helper);
}

void TypeBuilder::WalkBB(std::vector<BasicBlock*>& bbs, bool handleVarNodes)
{
	for (BasicBlock* bb : bbs)
	{
		PostOrderWalkerHelper<TypeBuilder*, Type> helper(this);
		for (auto& inst : *bb->getInstructions())
		{
			inst->PostOrderWalk(&helper);
		}
	}
}

Type TypeBuilder::WalkNode(InstructionNode*, std::optional<std::vector<Type>>)
{
	return Type::Unknown;
}

Type TypeBuilder::WalkNode(FlowControl*, Type, Type, std::optional<Type>)
{
	return Type::Unknown;
}

bool IsMoreSpecificType(Type old, Type newType)
{
	if (old == Type::Unknown)
		return newType != Type::Unknown;
	else if (old == Type::Dynamic)
		return newType != Type::Dynamic;

	assert(old == newType);//eg: int -> float shouldn't happen
	return false;
}

Type TypeBuilder::WalkNode(BinaryExpNode* node, Type left, Type right)
{
	if (node->token == Instructions::Assign)
	{
		assert(node->rightNode->GetType() == Node::VariableNode);
		
		if (!handleVarNodes)
			return Type::Unknown;
		
		std::shared_ptr<VariableNode> varNode = std::dynamic_pointer_cast<VariableNode>(node->rightNode);

		Type oldType = varNode->GetExpType();
		if (oldType == left)
			return left;

		assert(IsMoreSpecificType(oldType, left));
		node->SetExpType(left);
		varNode->SetExpType(left);
		IteratedNewInfo = true;

		return left;
	}
	
	if (left == Type::Unknown || right == Type::Unknown)
		return Type::Unknown;
	
	if (node->token == Instructions::Equals)
	{
		node->SetExpType(Type::Integer);
		return Type::Integer;
	}

	assert(node->token == Instructions::Add || node->token == Instructions::Multiply);

	assert(left != Type::Unknown);
	assert(right != Type::Unknown);

	if (left == right)
	{
		node->SetExpType(right);
		return right;
	}
	
	if (left == Type::Dynamic || right == Type::Dynamic)
	{
		node->SetExpType(Type::Dynamic);
		return Type::Dynamic;
	}

	if ((left == Type::Integer && right == Type::Float) ||
		(left == Type::Float && right == Type::Integer))
	{
		node->SetExpType(Type::Float);
		return Type::Float;
	}

	node->SetExpType(Type::Unknown);
	return Type::Unknown;
}

Type TypeBuilder::WalkNode(VariableNode* varNode)
{
	if (!handleVarNodes)
	{
		return Type::Unknown;
	}
		
	return varNode->GetExpType();
}

Type TypeBuilder::WalkNode(LiteralNode* l)
{
	Type t;

	if (std::holds_alternative<int>(l->literal))
		t = Type::Integer;
	else if (std::holds_alternative<float>(l->literal))
		t = Type::Float;
	else
	{
		t = Type::Unknown;
		assert(false);
	}

	l->SetExpType(t);
	return t;
}

Type TypeBuilder::WalkNode(LblNode*)
{
	return Type::Unknown;
}

Type TypeBuilder::WalkNode(GotoNode*)
{
	return Type::Unknown;
}

Type TypeBuilder::WalkNode(BasicJump*, Type, Type, std::optional<Type>)
{
	return Type::Unknown;
}
