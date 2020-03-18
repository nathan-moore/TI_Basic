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

};