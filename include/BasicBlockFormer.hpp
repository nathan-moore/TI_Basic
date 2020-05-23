#pragma once

#include "ASTNode.hpp"
#include "NodeVisitor.hpp"
#include <unordered_map>

class BasicBlockFormer {
	std::unordered_map<std::string, BasicBlock*> lookup;

	std::shared_ptr<GotoNode> ParseBranchBlock(std::unique_ptr<InstructionList>);
	BasicBlock* ParseBlocksInternal(std::unique_ptr<InstructionList>);
	void FormDominators(BasicBlock* entryPoint);
	
	//this owns the BasicBlocks until I write an arena allocator
	std::vector<BasicBlock*> bbs;
	int bbNum = 0;

public:
	BasicBlockFormer();

	BasicBlock* ParseBlocks(std::unique_ptr<InstructionList> list);

	std::vector<BasicBlock*>& getBBList() { return bbs; }
	void DumpBBs();
};