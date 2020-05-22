#include "SSAFormer.hpp"
#include "ASTNode.hpp"

#include <cassert>

void SSAFormer::FormState(std::vector<std::weak_ptr<BasicBlock>>& bbs)
{
	for (auto& weakbb : bbs)
	{
		auto bb = weakbb.lock();

		stateMapping.insert({ &*bb, {} });
		SSAState* state = &stateMapping[&*bb];

		for (auto& inst : *bb->getInstructions())
		{
			inst->PostOrderWalk(state);
		}
	}

	for (auto& weakbb : bbs)
	{
		auto bb = weakbb.lock();

		std::set<SSAVariable*> possibleSSAs;

		SSAState& state = stateMapping[&*bb];

		for (auto& pair : state.inVars)
		{
			if (pair.second.count == 0)
			{
				continue;
			}

			SSAVariable* var = FindDefinition(pair.first, bb);
			*pair.second.var = *var;
		}
	}
}

SSAVariable* SSAFormer::FindDefinition(Variable* var, std::shared_ptr<BasicBlock>& bb)
{
	return nullptr;
}

SSAFormer::SSAFormer()
{
}

void SSAFormer::FormSSABlocks(std::shared_ptr<BasicBlock>&)
{
}

void SSAState::WalkNode(BinaryExpNode* node)
{
	if (node->token != Instructions::Equals)
		return;

	assert(node->rightNode->GetType() == Node::VariableNode);
	std::shared_ptr<VariableNode> varNode = std::dynamic_pointer_cast<VariableNode>(node->rightNode);

	Variable* v = varNode->getVariable();
	InVarState& state = inVars[v];
	
	assert(state.count > 0);
	oldVars.push_back(state);

	inVars[v] = InVarState::GetNewState(this, v, node);
	state = inVars[v];

	varNode->SetSSAVariable(state.var);
}

void SSAState::WalkNode(VariableNode* var)
{
	//We assume that this isn't an assign, and deal with the 
	//case that it is above

	Variable* v = var->getVariable();

	if (outVars.count(v) > 0)
	{
		var->SetSSAVariable(outVars[v]);
		return;
	}

	if (inVars.count(v) == 0)
	{
		inVars.insert({ v, InVarState::GetNewState(this, v, nullptr) });
	}

	InVarState& state = inVars[v];
	state.AddUsage();
	var->SetSSAVariable(state.var);
}

//We don't care about these
void SSAState::WalkNode(InstructionNode*)
{
}

void SSAState::WalkNode(FlowControl*)
{
}

void SSAState::WalkNode(LiteralNode*)
{
}

void SSAState::WalkNode(LblNode*)
{
}

void SSAState::WalkNode(GotoNode*)
{
}

//Back to things we care about...

InVarState InVarState::GetNewState(SSAState*, Variable*, BinaryExpNode*)
{
	InVarState s;
	assert(false);
	return s;
}

void InVarState::AddUsage()
{
	count++;
}
