#include "SSAFormer.hpp"
#include "ASTNode.hpp"
#include "variable.hpp"

#include <cassert>

void SSAFormer::FormState(BasicBlock* bb)
{
	stateMapping.insert({ bb, {} });
	SSAState* state = &stateMapping[&*bb];

	for (auto& inst : *bb->getInstructions())
	{
		inst->PostOrderWalk(state);
	}

	for (auto& pair : state->outVars)
	{
		Variable* var = pair.first;
		if (containingBBs.count(var) == 0)
		{
			containingBBs.insert({ var, {} });
		}

		containingBBs[var].insert(bb);
	}
}

void SSAFormer::FlowState(Variable*)
{

}

SSAFormer::SSAFormer()
{
}

void SSAFormer::FormSSABlocks(std::vector<BasicBlock*>& bbs)
{
	for (BasicBlock* bb : bbs)
	{
		FormState(bb);
	}

	for (auto& v : containingBBs)
	{
		FlowState(v.first);
	}
}

void SSAState::WalkNode(BinaryExpNode* node)
{
	if (node->token != Instructions::Assign)
		return;

	assert(node->rightNode->GetType() == Node::VariableNode);
	std::shared_ptr<VariableNode> varNode = std::dynamic_pointer_cast<VariableNode>(node->rightNode);

	Variable* v = varNode->getVariable();
	InVarState& state = inVars[v];
	
	assert(state.count > 0);
	oldVars.push_back(state);

	inVars[v] = InVarState::GetNewState(v, node);
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
		inVars.insert({ v, InVarState::GetNewState(v, nullptr) });
	}

	InVarState& state = inVars[v];
	state.AddUsage();
	var->SetSSAVariable(state.var);
}

//We don't care about these
//TODO: if I ever add function calls, it also needs be handled here
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

void SSAState::WalkNode(BasicJump*)
{
}

//Back to things we care about...

InVarState InVarState::GetNewState(Variable* v, BinaryExpNode* node)
{
	InVarState s;

	int VN = UINT32_MAX;
	if (node != nullptr)
		VN = v->GetCount();

	SSAVariable* ssa = new SSAVariable(v, node, VN);
	s.var = ssa;
	s.count = 0;
	return s;
}

void InVarState::AddUsage()
{
	count++;
}
