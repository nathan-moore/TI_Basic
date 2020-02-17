#ifndef DRIVER_HH
# define DRIVER_HH

#include "Parser.tab.hh"

# define YY_DECL \
  yy::parser::symbol_type yylex (void)
// ... and declare it for the parser's sake.
YY_DECL;

#endif