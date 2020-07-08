#pragma once

#include "NodeVisitor.hpp"
#include "ASTNode.hpp"

#include <optional>
#include <stack>
#include <type_traits>

template<class T>
class TemplatedASTWalker {
public:
	virtual T WalkNode(InstructionNode*, std::optional<std::vector<T>>) = 0;
	virtual T WalkNode(FlowControl*, T Cond, T ifStatement, std::optional<T> elseStatement) = 0;
	virtual T WalkNode(BinaryExpNode*, T left, T right) = 0;
	virtual T WalkNode(VariableNode*) = 0;
	virtual T WalkNode(LiteralNode*) = 0;
	virtual T WalkNode(LblNode*) = 0;
	virtual T WalkNode(GotoNode*) = 0;
	virtual T WalkNode(BasicJump*, T Cond, T ifStatement, std::optional<T> elseStatement) = 0;
	virtual void WalkEndNode(T)
	{
		return;
	}
};

template< class T, class U>
class PostOrderWalkerHelper : public ASTWalker {
	std::stack<U> returnedArgs;
	T walker;

	void PushIfNotDefault(U temp)
	{
		if (!std::is_pointer<U>::value || temp != U())
			returnedArgs.push(temp);
	}
public:
	PostOrderWalkerHelper(T walker) :
		walker(walker)
	{
		//static_assert(std::is_base_of<TemplatedASTWalker<U>*, T>::value, "T is not derived from transformation");
	}

	// Inherited via ASTWalker
	virtual void WalkNode(InstructionNode* node) override
	{
		std::optional<std::vector<U>> dispArgs;
		if (node->instruction == Instructions::Disp)
		{
			auto& helper = std::get<DisplayHelper>(node->items);

			if (!helper.IsTriviableDisplayable())
			{
				int count = helper.GetVariadicCount();
				std::vector<U> vec(count);
				for (int i = 0; i < count; i++)
				{
					assert(returnedArgs.size() != 0);
					vec.insert(vec.begin(), returnedArgs.top());
					returnedArgs.pop();
				}

				dispArgs = vec;
			}
		}

		PushIfNotDefault(walker->WalkNode(node, dispArgs));
	}

	virtual void WalkNode(FlowControl* node) override
	{
		std::optional<U> arg2;

		if (node->getElseList() != nullptr)
		{
			arg2 = returnedArgs.top();
			returnedArgs.pop();
		}

		U arg1 = returnedArgs.top();
		returnedArgs.pop();

		U cond = returnedArgs.top();
		returnedArgs.pop();

		PushIfNotDefault(walker->WalkNode(node, cond, arg1, arg2));
	}
	virtual void WalkNode(BinaryExpNode* node) override
	{
		U left = returnedArgs.top();
		returnedArgs.pop();

		U right = returnedArgs.top();
		returnedArgs.pop();

		returnedArgs.push(walker->WalkNode(node, right, left));
	}

	virtual void WalkNode(VariableNode* node) override
	{
		PushIfNotDefault(walker->WalkNode(node));
	}
	virtual void WalkNode(LiteralNode* node) override
	{
		returnedArgs.push(walker->WalkNode(node));
	}
	virtual void WalkNode(LblNode* node) override
	{
		returnedArgs.push(walker->WalkNode(node));
	}
	virtual void WalkNode(GotoNode* node) override
	{
		returnedArgs.push(walker->WalkNode(node));
	}
	virtual void WalkNode(BasicJump* node) override
	{
		U ifCond = returnedArgs.top();
		returnedArgs.pop();

		std::optional<U> elseArg;
		
		if (node->getElseBB() != nullptr)
		{
			elseArg = returnedArgs.top();
			returnedArgs.pop();
		}
		
		U Condition = returnedArgs.top();
		returnedArgs.pop();

		returnedArgs.push(walker->WalkNode(node, Condition, ifCond, elseArg));
	}
	virtual void TopLevelNode() override
	{
		auto size = returnedArgs.size();
		assert(size == 0 || size == 1);

		if (size == 1)
		{
			U arg = returnedArgs.top();
			returnedArgs.pop();
			walker->WalkEndNode(arg);
		}
	}
};