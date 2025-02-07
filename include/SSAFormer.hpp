#ifndef SSAFormer__
#define SSAFormer__

#include "BasicBlock.hpp"
#include "NodeVisitor.hpp"
#include "ASTNode.hpp"

#include <memory>
#include <unordered_map>
#include <set>
#include <cstdint>

class SSAVariable {
public:
	uint32_t VN;
	BinaryExpNode* set;
	Variable* variable;
	Type t;

	SSAVariable(Variable* v, BinaryExpNode* node, uint32_t version)
		: VN(version), set(node), variable(v), t(Type::Unknown) {}

	SSAVariable()
		: VN(UINT32_MAX), set(nullptr), variable(nullptr), t(Type::Unknown) {}
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
public:
	SSAFormer();
	void FormSSABlocks(std::vector<BasicBlock*>& bbs);
};

#endif // !SSAFormer__
