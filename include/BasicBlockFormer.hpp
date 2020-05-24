#pragma once

#include "ASTNode.hpp"
#include "NodeVisitor.hpp"
#include <unordered_map>

class BasicBlockFormer {
	std::unordered_map<std::string, std::shared_ptr<LblNode>> lookup;
	std::set<std::shared_ptr<GotoNode>> fixUpNodes;

	std::shared_ptr<GotoNode> ParseBranchBlock(std::unique_ptr<InstructionList>, std::shared_ptr<LblNode>&);
	std::tuple<BasicBlock*, BasicBlock*> ParseBlocksInternal(std::unique_ptr<InstructionList>);
	void FormDominators(BasicBlock* entryPoint);
	std::string MakeLblName();
	void fixUpGotos();
	
	//this owns the BasicBlocks until I write an arena allocator
	std::vector<BasicBlock*> bbs;
	int bbNum = 0;
	int bbLabelNum = 0;

public:
	BasicBlockFormer();

	BasicBlock* ParseBlocks(std::unique_ptr<InstructionList> list);

	std::vector<BasicBlock*>& getBBList() { return bbs; }
	void DumpBBs();
};