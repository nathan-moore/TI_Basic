#pragma once

#include "NodeVisitor.hpp"
#include "ASTNode.hpp"

#include <optional>
#include <stack>
#include <type_traits>

template<class T>
class TemplatedASTWalker {
public:
	virtual T WalkNode(InstructionNode*) = 0;
	virtual T WalkNode(FlowControl*, T left , std::optional<T> right) = 0;
	virtual T WalkNode(BinaryExpNode*, T left, T right) = 0;
	virtual T WalkNode(VariableNode*) = 0;
	virtual T WalkNode(LiteralNode*) = 0;
	virtual T WalkNode(LblNode*) = 0;
	virtual T WalkNode(GotoNode*) = 0;
};

template<template<typename U> class T, class U>
class PostOrderWalkerHelper : ASTWalker {
	std::stack<U> returnedArgs;
	T<U> walker;

	PostOrderWalkerHelper(T<U> walker) :
		walker(walker)
	{
		static_assert(std::is_base_of<TemplatedASTWalker<U>, T<U>>::value, "T is not derived from transformation");
	}

	// Inherited via ASTWalker
	virtual void WalkNode(InstructionNode* node) override
	{
		returnedArgs.push(walker.WalkNode(node));
	}

	virtual void WalkNode(FlowControl* node) override
	{
		U arg1 = returnedArgs.Top();
		returnedArgs.pop();
		std::optional<U> arg2;

		if (node->getElseList() != nullptr)
		{
			arg2 = returnedArgs.top();
			returnedArgs.pop();
		}

		returnedArgs.push(walker.WalkNode(node, arg1, arg2));
	}
	virtual void WalkNode(BinaryExpNode* node) override
	{
		returnedArgs.push(walker.WalkNode(node));
	}
	virtual void WalkNode(VariableNode* node) override
	{
		returnedArgs.push(walker.WalkNode(node));
	}
	virtual void WalkNode(LiteralNode* node) override
	{
		returnedArgs.push(walker.WalkNode(node));
	}
	virtual void WalkNode(LblNode* node) override
	{
		returnedArgs.push(walker.WalkNode(node));
	}
	virtual void WalkNode(GotoNode* node) override
	{
		returnedArgs.push(walker.WalkNode(node));
	}
};