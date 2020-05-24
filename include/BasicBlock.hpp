#pragma once

#include <vector>
#include <memory>
#include <set>

#include <ASTNode.hpp>

class BasicBlock {
	friend class BasicBlockFormer;
	std::vector<BasicBlock*> preBlocks;
	std::vector<BasicBlock*> postBlocks;
	std::set<BasicBlock*> dominators;
	std::set<BasicBlock*> dominanceFrontier;

	std::unique_ptr<InstructionList> instructions;
	int bbNum;
public:
	BasicBlock(int num) 
		:instructions(std::make_unique<InstructionList>()), bbNum(num)
	{}

	BasicBlock(std::unique_ptr<InstructionList> instructions, int num)
		: instructions(std::move(instructions)), bbNum(num) {}

	void AddPreBlock(BasicBlock* block)
	{
		assert(block != nullptr);
		preBlocks.push_back(block);
	}

	void AddPostBlock(BasicBlock* block)
	{
		assert(block != nullptr);
		postBlocks.push_back(block);
	}

	std::unique_ptr<InstructionList>& getInstructions()
	{
		return instructions;
	}

	const std::unique_ptr<InstructionList>& getInstructions() const
	{
		return instructions;
	}

	int GetBBNum()
	{
		return bbNum;
	}

	using BBIter = std::set<BasicBlock*>::iterator;

	BBIter begin()
	{
		return dominators.begin();
	}

	BBIter end()
	{
		return dominators.end();
	}

	BBIter DFbegin()
	{
		return dominanceFrontier.begin();
	}

	BBIter DFend()
	{
		return dominanceFrontier.end();
	}
};