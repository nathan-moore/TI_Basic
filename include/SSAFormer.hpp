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
	bool isPhi;
	SSAVariable* v1;
	SSAVariable* v2;

	uint32_t VN;
	BinaryExpNode* set;
	Variable* variable;

	SSAVariable(Variable* v, BinaryExpNode* node, uint32_t version)
		: isPhi(false), v1(nullptr), v2(nullptr), VN(version), set(node), variable(v) {}

	SSAVariable()
		: v1(nullptr), v2(nullptr), VN(UINT32_MAX), set(nullptr), variable(nullptr) {}
};

class SSAState;

class InVarState {
public:
	SSAVariable* var;
	int count;

	static InVarState GetNewState(Variable* v, BinaryExpNode* node);

	void AddUsage();
};

class SSAFormer;

//Per BB internal state
class SSAState : public ASTWalker {
public:
	//TODO: perf/correctness arena allocator
	std::unordered_map<Variable*, InVarState> inVars; //Variables for a BB that need to be matched
	std::unordered_map<Variable*, SSAVariable*> outVars;
	std::vector<InVarState> oldVars;

	BasicBlock* boundBB;
	SSAFormer* former;

	SSAState(BasicBlock* bb, SSAFormer* f)
		: boundBB(bb), former(f) {}

	// Inherited via ASTWalker
	virtual void WalkNode(InstructionNode*) override;
	virtual void WalkNode(FlowControl*) override;
	virtual void WalkNode(BinaryExpNode*) override;
	virtual void WalkNode(VariableNode*) override;
	virtual void WalkNode(LiteralNode*) override;
	virtual void WalkNode(LblNode*) override;
	virtual void WalkNode(GotoNode*) override;
	virtual void WalkNode(BasicJump*) override;
	//variables exported by the bb
};

class SSAFormer {
private:
	friend class SSAState;
	std::unordered_map<BasicBlock*, SSAState> stateMapping;
	std::unordered_map<Variable*, std::set<BasicBlock*>> containingBBs;
	std::unordered_map<Variable*, std::set<BasicBlock*>> defBBs;
	
	void FlowState(Variable* v);
	void FormState(BasicBlock* bb);
	void InsertPhiNode(Variable* v, BasicBlock* bb);
	SSAVariable* GrabDef(Variable* v, BasicBlock*);
public:
	SSAFormer();
	void FormSSABlocks(std::vector<BasicBlock*>& bbs);
};

#endif // !SSAFormer__
