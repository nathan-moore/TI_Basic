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
	auto [topLevelBB, _] = ParseBlocksInternal(std::move(list));
	fixUpGotos();
	fixUpPreds();

	DumpBBs();
	ASNodePrinter p;
	p.WalkBBs(topLevelBB);

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
std::tuple<BasicBlock*, BasicBlock*> BasicBlockFormer::ParseBlocksInternal(std::unique_ptr<InstructionList> list)
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

		auto newBB = new BasicBlock(bbNum++);
		bbs.push_back(newBB);

		std::shared_ptr<LblNode> lbl = std::make_shared<LblNode>(MakeLblName(), newBB);
		newBB->getInstructions()->push_back(lbl);

		if (type == Node::LblNode ||
			type == Node::GotoNode)
		{
			if (type == Node::GotoNode)
			{
				current->getInstructions()->push_back(node);
				fixUpNodes.insert({ std::static_pointer_cast<GotoNode>(node), current });
			}
			else
			{
				newBB->getInstructions()->push_back(node); //We push along the node to allow for more information dumping
				std::shared_ptr<LblNode> Lblnode = std::static_pointer_cast<LblNode>(node);
				lookup.insert({ Lblnode->GetLbl(), Lblnode });
				Lblnode->SetBB(newBB);
			}

			std::shared_ptr<GotoNode> gotoNode = std::make_shared<GotoNode>(lbl);
			current->getInstructions()->push_back(gotoNode);
			current->AddPostBlock(newBB);

			//We don't have enough information to propegate edges here, so give up and come back later for a second pass
		}
		else if (type == Node::FlowControl)
		{
			std::shared_ptr<FlowControl> cntrl = std::static_pointer_cast<FlowControl>(node);

			std::shared_ptr<GotoNode> elseBranch;
			if (cntrl->getElseList() == nullptr)
			{
				elseBranch = std::make_shared<GotoNode>(lbl);
			}
			else
			{
				elseBranch = ParseBranchBlock(cntrl->moveElseList(), lbl);
			}

			std::shared_ptr<BasicJump> jmp = std::make_shared<BasicJump>(cntrl->getCond(),
				ParseBranchBlock(cntrl->moveIfList(), lbl),
				elseBranch);

			current->getInstructions()->push_back(jmp);

			current->AddPostBlock(jmp->getIfBB());
			current->AddPostBlock(jmp->getElseBB());
		}

		current = newBB;
	}

	return std::make_tuple(first, current);
}

std::shared_ptr<GotoNode> BasicBlockFormer::ParseBranchBlock(std::unique_ptr<InstructionList> list, std::shared_ptr<LblNode>& nxtLbl)
{
	if (list == nullptr)
		return nullptr;

	assert(nxtLbl->GetBB() != nullptr);

	auto [firstBB, lastBB] = ParseBlocksInternal(std::move(list));

	std::shared_ptr<LblNode> lblNode = std::make_shared<LblNode>(MakeLblName(), firstBB);
	firstBB->getInstructions()->push_front(lblNode);

	std::shared_ptr<GotoNode> gotoNode = std::make_shared<GotoNode>(nxtLbl);
	lastBB->getInstructions()->push_back(gotoNode);
	lastBB->AddPostBlock(nxtLbl->GetBB());
	return std::make_shared<GotoNode>(lblNode);
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

			toSet.insert(bb);

			if (toSet != bb->dominators)
			{
				changed = true;
				bb->dominators = toSet;
			}
		}
	}
}

std::string BasicBlockFormer::MakeLblName()
{
	return std::string("BBLabel") + std::to_string(bbLabelNum++);
}

void BasicBlockFormer::fixUpGotos()
{
	while (fixUpNodes.size() != 0)
	{
		auto both = *fixUpNodes.begin();
		fixUpNodes.erase(both);
		auto [toFixUp, containingBB] = both;

		if (toFixUp->HasBB())
			continue;

		auto LblNode = lookup[toFixUp->GetStr()];
		toFixUp->SetLabel(LblNode);
		containingBB->AddPostBlock(LblNode->GetBB());
	}
}

void BasicBlockFormer::fixUpPreds()
{
	for (BasicBlock* bb : bbs)
	{
		for (BasicBlock* post : bb->postBlocks)
		{
			post->AddPreBlock(bb);
		}
	}
}
