#ifndef SSAFormer__
#define SSAFormer__

#include "BasicBlock.hpp"
#include "NodeVisitor.hpp"

#include <memory>
#include <unordered_map>
#include <set>
#include <cstdint>

class SSAVariable {
public:
	uint32_t VN;
	BinaryExpNode* set;
	Variable* variable;
};

class SSAState;

class InVarState {
public:
	SSAVariable* var;
	int count;

	static InVarState GetNewState(SSAState* state, Variable* v, BinaryExpNode* node);

	void AddUsage();
};

//Per BB internal state
class SSAState : public ASTWalker {
public:
	//TODO: perf/correctness arena allocator
	std::unordered_map<Variable*, InVarState> inVars; //Variables for a BB that need to be matched
	std::unordered_map<Variable*, SSAVariable*> outVars;
	std::vector<InVarState> oldVars;

	// Inherited via ASTWalker
	virtual void WalkNode(InstructionNode*) override;
	virtual void WalkNode(FlowControl*) override;
	virtual void WalkNode(BinaryExpNode*) override;
	virtual void WalkNode(VariableNode*) override;
	virtual void WalkNode(LiteralNode*) override;
	virtual void WalkNode(LblNode*) override;
	virtual void WalkNode(GotoNode*) override;
	//variables exported by the bb
};

class SSAFormer {
private:
	std::unordered_map<BasicBlock*, SSAState> stateMapping;
	void FormState(std::vector<std::weak_ptr<BasicBlock>>& bbs);
	SSAVariable* FindDefinition(Variable* var, std::shared_ptr<BasicBlock>& bb);
public:
	SSAFormer();
	void FormSSABlocks(std::shared_ptr<BasicBlock>& bblocks);
};

#endif // !SSAFormer__
