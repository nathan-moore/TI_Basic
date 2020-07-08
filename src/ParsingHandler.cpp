#include "Driver.hh"
#include "BasicBlockFormer.hpp"
#include "SSAFormer.hpp"
#include "TypeBuilder.hpp"
#include "llvmJit.hpp"

#include <cstdlib>
#include <iostream>
#include <cassert>

#include <llvm/Support/TargetRegistry.h>

#include <llvm/IR/Module.h>

extern FILE* yyin;

namespace yy{

void parser::error(const std::string& str)
{
    std::cout << "Failure at " << str << std::endl;
    throw std::runtime_error("Failure at " + str + "\n");
}

}

driver::driver()
: result(-1),
    module(std::make_unique<llvm::Module>("TIBasicSomething", context))
{  
    using namespace llvm;

    // Initialize the target registry etc.
    llvm::InitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();

    auto TargetTriple = sys::getDefaultTargetTriple();
    module->setTargetTriple(TargetTriple);

    std::string error;
    auto target = TargetRegistry::lookupTarget(TargetTriple, error);

    if (!target)
    {
        std::cout << error;
        exit(1);
    }

    auto CPU = "generic";
    auto features = "";

    TargetOptions opt;
    auto RM = Optional<llvm::Reloc::Model>();
    auto theTargetMachine = target->createTargetMachine(TargetTriple, CPU, features, opt, RM);
    module->setDataLayout(theTargetMachine->createDataLayout());
}

int driver::parse(FILE* f)
{
    yyin = f;
    yy::parser parse(*this);
    //parse.set_debug_level(1);
    result = parse();
    fclose(f);
    return result;
}


int driver::parseString(const std::string& str)
{
    FILE* f = fmemopen((void*)str.c_str(), str.length(), "r");
    return parse(f);
}

void driver::Compile()
{
    assert(topNode != nullptr);

    TypeBuilder b;
    b.WalkNode(topNode, false);

    BasicBlockFormer former;
    bbs = former.ParseBlocks(std::move(topNode));
    former.DumpBBs();

    SSAFormer ssaFormer;
    ssaFormer.FormSSABlocks(former.getBBList());

    b.WalkNode((*bbs->begin())->getInstructions(), true); //TODO: fixme

    ASNodePrinter printer;
    printer.WalkBBs(bbs);

    IRGen gen(&context, module);
    gen.FormIR(bbs, former.getBBList());
    module = gen.MoveModule();
}

voidFunc driver::EmitCode()
{
    std::unique_ptr<llvmInMemJit> jit = llvmInMemJit::makeJit(std::move(module));
    return jit->comFunction("Func");
}

int driver::parse(const std::string& fileName)
{
    FILE* f = fopen(fileName.c_str(), "r");
    return parse(f);
}

