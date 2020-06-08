#ifndef llvmInMemCompiler
#define llvmInMemCompiler

#include "IRGen.hpp"
#include "Driver.hh"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/Support/TargetSelect.h"


class llvmInMemJit {
	llvm::ExecutionEngine* engine;

public:
	llvmInMemJit(llvm::ExecutionEngine* e)
		:engine(e)
	{

	}

	static std::unique_ptr<llvmInMemJit> makeJit(std::unique_ptr<llvm::Module> mod)
	{
		std::string error;
		llvm::ExecutionEngine* engine = llvm::EngineBuilder(std::move(mod))
			.setErrorStr(&error)
			.setEngineKind(llvm::EngineKind::JIT)
			.setOptLevel(llvm::CodeGenOpt::Default)
			.create();

		if (engine == nullptr)
		{
			std::cout << "Building engine failed with: " << error << std::endl;
			exit(1);
		}

		return std::make_unique<llvmInMemJit>(engine);
	}

	voidFunc comFunction(const std::string& name)
	{
		return (voidFunc)engine->getFunctionAddress(name);
	}
};


#endif // !llvmInMemCompiler

