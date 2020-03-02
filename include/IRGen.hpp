#pragma once

#include "PostOrderWalkerHelper.hpp"

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

class GenData : public TemplatedASTWalker<LLVM::Value*>
{
	LLVMContext context;
	Module module;

	GenData()
		: module("TI Basic Something", context)
	{

	}

};