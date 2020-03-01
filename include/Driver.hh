#ifndef DRIVER_HH
#define DRIVER_HH

#include "Grammer.tab.hh"
#include "stdio.h"

class driver
{
private:
  int parse(FILE* f);
public:
  int result;
  std::shared_ptr<InstructionList> topNode;
  
  driver();
  int parse (const std::string& f);
  int parseString(const std::string& f);
  void noop() {}
};

# define YY_DECL \
  yy::parser::symbol_type yylex (driver& drv)
// ... and declare it for the parser's sake.
YY_DECL;

#endif