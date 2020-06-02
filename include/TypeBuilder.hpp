#include "PostOrderWalkerHelper.hpp"
#include "ASTNode.hpp"

class TypeBuilder : TemplatedASTWalker<Type>
{
public:
	void WalkNode(std::unique_ptr<InstructionList>&);

	// Inherited via TemplatedASTWalker
	virtual Type WalkNode(InstructionNode*) override;
	virtual Type WalkNode(FlowControl*, Type Cond, Type ifStatement, std::optional<Type> elseStatement) override;
	virtual Type WalkNode(BinaryExpNode*, Type left, Type right) override;
	virtual Type WalkNode(VariableNode*) override;
	virtual Type WalkNode(LiteralNode*) override;
	virtual Type WalkNode(LblNode*) override;
	virtual Type WalkNode(GotoNode*) override;
	virtual Type WalkNode(BasicJump*, Type Cond, Type ifStatement, std::optional<Type> elseStatement) override;
};