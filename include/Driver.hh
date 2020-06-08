#ifndef DRIVER_HH
#define DRIVER_HH

#include "Grammer.tab.hh"
#include "stdio.h"
#include "SymbolTable.hpp"

#include <llvm/IR/Module.h>

typedef void (*voidFunc)();

class driver
{
private:
  int parse(FILE* f);
  
public:
  int result;
  std::unique_ptr<InstructionList> topNode;
  BasicBlock* bbs;
  SymbolTable table;
  llvm::LLVMContext context;
  std::unique_ptr<llvm::Module> module;
  
  driver();
  int parse (const std::string& f);
  int parseString(const std::string& f);
  void noop() {}
  void Compile();
  voidFunc EmitCode();
};

# define YY_DECL \
  yy::parser::symbol_type yylex (driver& drv)
// ... and declare it for the parser's sake.
YY_DECL;

#endif