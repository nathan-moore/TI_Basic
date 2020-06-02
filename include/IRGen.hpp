#pragma once

#include "PostOrderWalkerHelper.hpp"
#include "BasicBlock.hpp"

#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

class IRGen : public TemplatedASTWalker<llvm::Value*>
{
	llvm::LLVMContext context;
	llvm::Module module;

	IRGen()
		: module("TI Basic Something", context)
	{

	}

	void FormIR(std::shared_ptr<BasicBlock>);


	// Inherited via TemplatedASTWalker
	virtual llvm::Value* WalkNode(InstructionNode*) override;

	virtual llvm::Value* WalkNode(FlowControl*, llvm::Value* Cond, llvm::Value* ifStatement, std::optional<llvm::Value*> elseStatement) override;

	virtual llvm::Value* WalkNode(BinaryExpNode*, llvm::Value* left, llvm::Value* right) override;

	virtual llvm::Value* WalkNode(VariableNode*) override;

	virtual llvm::Value* WalkNode(LiteralNode*) override;

	virtual llvm::Value* WalkNode(LblNode*) override;

	virtual llvm::Value* WalkNode(GotoNode*) override;

	virtual llvm::Value* WalkNode(BasicJump*, llvm::Value* Cond, llvm::Value* ifStatement, std::optional<llvm::Value*> elseStatement) override;

};