#include <ASTNode.hpp>
#include <BasicBlock.hpp>
#include <NodeVisitor.hpp>
#include <cassert>
#include "BasicBlockFormer.hpp"

BasicBlockFormer::BasicBlockFormer()
{
}

std::shared_ptr<BasicBlock> BasicBlockFormer::ParseBlocks(std::unique_ptr<InstructionList> list)
{
	std::shared_ptr<BasicBlock> current = std::make_shared<BasicBlock>();
	std::shared_ptr<BasicBlock> first = current;

	std::vector<std::shared_ptr<BasicBlock>> last;
	for (std::shared_ptr<AstNode>& node : *list)
	{
		auto type = node->GetType();
		assert(type != Node::BasicJump);// We form these here	

		if (type != Node::LblNode &&
			type != Node::FlowControl &&
			type != Node::GotoNode)
		{
			current->getInstructions()->push_back(node);
			continue;
		}

		std::vector<std::shared_ptr<BasicBlock>> updateLast;
		auto newBB = std::make_shared<BasicBlock>(std::move(list));

		if (type == Node::LblNode ||
			type == Node::GotoNode)
		{
			if (type == Node::GotoNode)
				current->getInstructions()->push_back(node);

			updateLast.push_back(current);
		}
		else if (type == Node::FlowControl)
		{
			std::shared_ptr<FlowControl> cntrl = std::static_pointer_cast<FlowControl>(node);

			std::shared_ptr<BasicJump> jmp = std::make_shared<BasicJump>(cntrl->getCond(),
				ParseBranchBlock(std::move(cntrl->getIfList())),
				ParseBranchBlock(std::move(cntrl->getElseList())));

			current->getInstructions()->push_back(jmp);
			updateLast.push_back(jmp->getIfBB());
			updateLast.push_back(jmp->getElseBB());
		}

		for (std::shared_ptr<BasicBlock>& n : last)
		{
			newBB->AddPreBlock(n);
			n->AddPostBlock(newBB);
		}
		last.clear();
		last.push_back(current);

		current = newBB;

		if (type == Node::LblNode)
		{
			std::shared_ptr<LblNode> node = std::static_pointer_cast<LblNode>(node);
			lookup.insert({ node->GetLbl(), last.at(0) });
		}
	}

	return first;
}

std::shared_ptr<GotoNode> BasicBlockFormer::ParseBranchBlock(std::unique_ptr<InstructionList> list)
{
	std::shared_ptr<BasicBlock> bb = ParseBlocks(std::move(list));
	return std::make_shared<GotoNode>(bb);
}

