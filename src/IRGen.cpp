#include "IRGen.hpp"

#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include <llvm/IR/Verifier.h>

#include <iostream>
#include <unistd.h>
#include <stdio.h>

void IRGen::FormIR(BasicBlock* start, std::vector<BasicBlock*>& list)
{
	assert(list.size() == 1 && "Don't support multiple bbs yet");

	llvm::FunctionType* proto = llvm::FunctionType::get(llvm::Type::getVoidTy(*context), false);
	llvm::Function* fc = llvm::Function::Create(proto, llvm::Function::ExternalLinkage, "Func", *module);

	currBB = llvm::BasicBlock::Create(*context, "entry",fc);
	builder.SetInsertPoint(currBB);
	PostOrderWalkerHelper<IRGen*, llvm::Value*> helper(this);
	start->getInstructions()->PostOrderWalk(&helper);

	builder.CreateRetVoid();
	
	llvm::raw_fd_ostream stream(STDOUT_FILENO, false);
	if (verifyFunction(*fc, &stream))
	{
		std::cout << "Error in ir" << std::endl;
		module->print(llvm::errs(), nullptr);
		exit(1);
	}
}

llvm::Value* IRGen::WalkNode(InstructionNode* node)
{
	if (node->instruction == Instructions::Disp)
	{
		llvm::Function* func_printf = module->getFunction("printf");

		if (!func_printf) {
			llvm::FunctionType* FuncTy9 = llvm::FunctionType::get(llvm::IntegerType::get(*context, 32), true);

			func_printf = llvm::Function::Create(FuncTy9, llvm::GlobalValue::ExternalLinkage, "printf", *module);
			func_printf->setCallingConv(llvm::CallingConv::C);
		}

		//TODO: don't handle only str
		std::vector<llvm::Value*> callParams;
		callParams.push_back(builder.CreateGlobalStringPtr("%s\n"));
		callParams.push_back(builder.CreateGlobalStringPtr(std::get<std::string>(node->items).c_str()));
		//llvm::CallInst::Create(func_printf, callParams, "call", currBB);
		builder.CreateCall(func_printf, callParams);
		return nullptr;
	}
	else
	{
		assert(false);
	}
	return nullptr;
}

llvm::Value* IRGen::WalkNode(FlowControl*, llvm::Value*, llvm::Value*, std::optional<llvm::Value*>)
{
	return nullptr;
}

llvm::Value* IRGen::WalkNode(BinaryExpNode* node, llvm::Value* lhs, llvm::Value* rhs)
{
	switch (node->token)
	{
	case Instructions::Add:
		if (node->GetExpType() == Type::Float)
		{
			return builder.CreateFAdd(lhs, rhs);
		}
		else
		{
			assert(node->GetExpType() == Type::Integer);
			return builder.CreateAdd(lhs, rhs);
		}
	case Instructions::Multiply:
		if (node->GetExpType() == Type::Float)
		{
			return builder.CreateMul(lhs, rhs);
		}
		else
		{
			assert(node->GetExpType() == Type::Float);
			return builder.CreateFMul(lhs, rhs);
		}
	case Instructions::Equals:
		if (node->GetExpType() == Type::Float)
		{
			return builder.CreateFCmpOEQ(lhs, rhs);
		}
		else
		{
			return builder.CreateICmpEQ(lhs, rhs);
		}
	default:
		break;
	}

	//TODO: Assign
	assert(false);
	return nullptr;
}

llvm::Value* IRGen::WalkNode(VariableNode*)
{
	return nullptr;
}

llvm::Value* IRGen::WalkNode(LiteralNode* literal)
{
	assert(literal->GetExpType() != Type::Dynamic &&
		literal->GetExpType() != Type::Unknown);

	if (literal->GetExpType() == Type::Float)
	{
		llvm::Type* t = llvm::Type::getFloatTy(*context);
		return llvm::ConstantFP::get(t, std::get<float>(literal->literal));
	}
	else if (literal->GetExpType() == Type::Integer)
	{
		llvm::Type* t = llvm::Type::getInt32Ty(*context);
		return llvm::ConstantInt::get(t, std::get<int>(literal->literal));
	}

	assert(false);
	return nullptr;
}

llvm::Value* IRGen::WalkNode(LblNode*)
{
	return nullptr;
}

llvm::Value* IRGen::WalkNode(GotoNode*)
{
	return nullptr;
}

llvm::Value* IRGen::WalkNode(BasicJump*, llvm::Value*, llvm::Value*, std::optional<llvm::Value*>)
{
	return nullptr;
}

