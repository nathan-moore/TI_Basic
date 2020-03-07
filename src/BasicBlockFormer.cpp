#include <ASTNode.hpp>
#include <BasicBlock.hpp>
#include <NodeVisitor.hpp>
#include <cassert>
#include "BasicBlockFormer.hpp"

BasicBlockFormer::BasicBlockFormer()
{
}

//TODO: refactor?
std::shared_ptr<BasicBlock> BasicBlockFormer::ParseBlocks(std::unique_ptr<InstructionList> list)
{
	std::shared_ptr<BasicBlock> first = std::make_shared<BasicBlock>();

	std::vector<std::shared_ptr<BasicBlock>> last;
	for (std::shared_ptr<AstNode>& node : *list)
	{
		auto type = node->GetType();
		assert(type != Node::BasicJump);// We form these here

		if (type != Node::LblNode &&
			type != Node::FlowControl &&
			type != Node::GotoNode)
		{
			first->getInstructions()->push_back(node);
			continue;
		}

		if (type == Node::LblNode ||
			type == Node::GotoNode)
		{
			if (type == Node::GotoNode)
				first->getInstructions()->push_back(node);

			std::shared_ptr<BasicBlock> block = std::make_shared<BasicBlock>(std::move(list));
			if (first == nullptr)
				first = block;
			
			for (std::shared_ptr<BasicBlock>& n : last)
			{
				block->AddPreBlock(n);
			}
			last.clear();
			last.push_back(block);
		}
		else if (type == Node::FlowControl)
		{
			std::shared_ptr<FlowControl> cntrl = std::static_pointer_cast<FlowControl>(node);

			std::shared_ptr<BasicJump> jmp = std::make_shared<BasicJump>(cntrl->getCond(),
				ParseBranchBlock(std::move(cntrl->getIfList())),
				ParseBranchBlock(std::move(cntrl->getElseList())));

			first->getInstructions()->push_back(jmp);

			for (std::shared_ptr<BasicBlock>& n : last)
			{
				//TODO: add in basic blocks
				assert(false);
			}
		}


		if (type == Node::LblNode)
		{
			std::shared_ptr<LblNode> node = std::static_pointer_cast<LblNode>(node);
			lookup.insert({ node->GetLbl(), last.at(0) });
		}
	}
	return first;
}

