#pragma once

#include "ASTNode.hpp"
#include "NodeVisitor.hpp"
#include <unordered_map>

class BasicBlockFormer {
	std::unordered_map<std::string, std::shared_ptr<BasicBlock>> lookup;
public:
	BasicBlockFormer();

	std::shared_ptr<BasicBlock> ParseBlocks(std::unique_ptr<InstructionList>);

	std::shared_ptr<GotoNode> ParseBranchBlock(std::unique_ptr<InstructionList>);

};