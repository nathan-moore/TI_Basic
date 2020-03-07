#pragma once

#include <vector>
#include <memory>

#include <ASTNode.hpp>

class BasicBlock {
	std::vector<std::weak_ptr<BasicBlock>> preBlocks;
	std::vector<std::shared_ptr<BasicBlock>> postBlocks;

	std::unique_ptr<InstructionList> instructions;
public:
	BasicBlock() {}

	BasicBlock(std::unique_ptr<InstructionList> instructions)
		: instructions(std::move(instructions)) {}

	void AddPreBlock(std::shared_ptr<BasicBlock> block)
	{
		preBlocks.push_back(block);
	}

	std::unique_ptr<InstructionList>& getInstructions()
	{
		return instructions;
	}

	const std::unique_ptr<InstructionList>& getInstructions() const
	{
		return instructions;
	}
};