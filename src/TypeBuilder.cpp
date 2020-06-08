#include "TypeBuilder.hpp"

void TypeBuilder::WalkNode(std::unique_ptr<InstructionList>& node)
{
	PostOrderWalkerHelper<TypeBuilder*, Type> helper(this);
	node->PostOrderWalk(&helper);
}

Type TypeBuilder::WalkNode(InstructionNode*)
{
	return Type::Unknown;
}

Type TypeBuilder::WalkNode(FlowControl*, Type, Type, std::optional<Type>)
{
	return Type::Unknown;
}

Type TypeBuilder::WalkNode(BinaryExpNode* node, Type left, Type right)
{
	if (node->token == Instructions::Assign)
	{
		assert(node->rightNode->GetType() == Node::VariableNode);
		std::shared_ptr<VariableNode> varNode = std::dynamic_pointer_cast<VariableNode>(node->rightNode);

		//TODO: Dynamic
		if (varNode->IsExpTypeSet())
		{
			assert(varNode->GetExpType() == left);
		}

		varNode->SetExpType(left);
		
		node->SetExpType(left);
		return left;
	}
	else if (node->token == Instructions::Equals)
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

Type TypeBuilder::WalkNode(VariableNode*)
{
	//TODO?
	return Type::Dynamic;
}

Type TypeBuilder::WalkNode(LiteralNode* l)
{
	if (std::holds_alternative<int>(l->literal))
		return Type::Integer;
	else if (std::holds_alternative<float>(l->literal))
		return Type::Float;
	else
		return Type::Unknown;
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
