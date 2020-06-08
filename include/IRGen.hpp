#pragma once

#include "PostOrderWalkerHelper.hpp"
#include "BasicBlock.hpp"

#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

class IRGen : public TemplatedASTWalker<llvm::Value*>
{
	std::unique_ptr<llvm::Module>& module;
	llvm::IRBuilder<> builder;
	llvm::LLVMContext* context;
	llvm::BasicBlock* currBB;

public:
	IRGen(llvm::LLVMContext* c, std::unique_ptr<llvm::Module>& mod)
		: module(mod),
		builder(*c),
		context(c)
	{

	}

	void FormIR(BasicBlock*, std::vector<BasicBlock*>&);

	std::unique_ptr<llvm::Module> MoveModule()
	{
		return std::move(module);
	}

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