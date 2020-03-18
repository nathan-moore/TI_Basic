#pragma once

#include "ASTNode.hpp"
#include "NodeVisitor.hpp"
#include <unordered_map>

class BasicBlockFormer {
	std::unordered_map<std::string, std::shared_ptr<BasicBlock>> lookup;

	std::shared_ptr<GotoNode> ParseBranchBlock(std::unique_ptr<InstructionList>);

public:
	BasicBlockFormer();

	std::shared_ptr<BasicBlock> ParseBlocks(std::unique_ptr<InstructionList>);
};