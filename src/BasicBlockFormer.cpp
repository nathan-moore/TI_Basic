#include <ASTNode.hpp>
#include <BasicBlock.hpp>
#include <NodeVisitor.hpp>
#include <cassert>
#include <algorithm>
#include <iostream>

#include "BasicBlockFormer.hpp"

BasicBlockFormer::BasicBlockFormer()
{
}

BasicBlock* BasicBlockFormer::ParseBlocks(std::unique_ptr<InstructionList> list)
{
	BasicBlock* topLevelBB = ParseBlocksInternal(std::move(list));
	DumpBBs();
	FormDominators(topLevelBB);

	return topLevelBB;
}

void BasicBlockFormer::DumpBBs()
{
	for (BasicBlock* bb : bbs)
	{
		std::cout << "bb Num is: " << bb->bbNum << std::endl;
		std::cout << "Preds are: ";
		for (BasicBlock* bbPred : bb->preBlocks)
		{
			std::cout << bbPred->bbNum << " ";
		}
		std::cout << std::endl;

		std::cout << "PostBlocks are: ";
		for (BasicBlock* bbPost : bb->postBlocks)
		{
			std::cout << bbPost->bbNum << " ";
		}
		std::cout << std::endl << std::endl;
	}
}

//TODO: arena allocators for the BBs
BasicBlock* BasicBlockFormer::ParseBlocksInternal(std::unique_ptr<InstructionList> list)
{
	BasicBlock* current = new BasicBlock(bbNum++);
	bbs.push_back(current);
	auto first = current;

	for (std::shared_ptr<AstNode> node : *list)
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

		std::vector<BasicBlock*> last;
		//We modify list here, so can't touch the iterator after this
		auto newBB = new BasicBlock(bbNum++);
		bbs.push_back(newBB);

		if (type == Node::LblNode ||
			type == Node::GotoNode)
		{
			if (type == Node::GotoNode)
				current->getInstructions()->push_back(node);

			last.push_back(newBB);
		}
		else if (type == Node::FlowControl)
		{
			std::shared_ptr<FlowControl> cntrl = std::static_pointer_cast<FlowControl>(node);

			std::shared_ptr<BasicJump> jmp = std::make_shared<BasicJump>(cntrl->getCond(),
				ParseBranchBlock(cntrl->moveIfList()),
				ParseBranchBlock(cntrl->moveElseList()));

			current->getInstructions()->push_back(jmp);
			last.push_back(jmp->getIfBB());
			last.push_back(jmp->getElseBB());
		}

		for (BasicBlock* n : last)
		{
			newBB->AddPreBlock(n);
			current->AddPostBlock(n);
		
			n->AddPostBlock(newBB);
			n->AddPreBlock(current);
		}

		current = newBB;

		if (type == Node::LblNode)
		{
			std::shared_ptr<LblNode> Lblnode = std::static_pointer_cast<LblNode>(node);
			lookup.insert({ Lblnode->GetLbl(), last.at(0) });
		}
	}

	return first;
}

std::shared_ptr<GotoNode> BasicBlockFormer::ParseBranchBlock(std::unique_ptr<InstructionList> list)
{
	BasicBlock* bb = ParseBlocksInternal(std::move(list));
	return std::make_shared<GotoNode>(bb);
}

//A really simple and inefficient Dominator calculation algorithm
//1st one here https://www.cs.au.dk/~gerth/advising/thesis/henrik-knakkegaard-christensen.pdf
void BasicBlockFormer::FormDominators(BasicBlock* entryPoint)
{
	std::set<BasicBlock*> fullSet;

	for (BasicBlock* bb : bbs)
	{
		fullSet.insert(bb);
	}

	for (BasicBlock* bb : bbs)
	{
		bb->dominators = fullSet;
	}

	entryPoint->dominators.clear();
	entryPoint->dominators.insert(entryPoint);

	bool changed = true;
	while (changed)
	{
		changed = false;

		for (BasicBlock* bb : bbs)
		{
			if (bb == entryPoint)
				continue;

			std::set<BasicBlock*> toSet;

			assert(bb->preBlocks.size() > 0);
			toSet = bb->preBlocks[0]->dominators;

			for (size_t i = 1; i < bb->preBlocks.size(); ++i)
			{
				std::set<BasicBlock*>& toUnion = bb->preBlocks[i]->dominators;
				std::set<BasicBlock*> temp;
				std::set_intersection(toSet.begin(), toSet.end(), toUnion.begin(), toUnion.end(), 
					std::inserter(temp, temp.begin()));
				toSet = temp;
			}

			if (toSet != bb->dominators)
			{
				changed = true;
				bb->dominators = toSet;
			}
		}
	}
}
