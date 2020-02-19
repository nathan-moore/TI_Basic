%{
    #include "Driver.hh"
    #include "Parser.tab.hh"

    using namespace yy;
%}

%option noyywrap nounput noinput batch debug


%%

"=" return parser::make_T_Equals();
\"[^\"\n]*[\"\n] return parser::make_T_String(yytext);
"Disp " return parser::make_T_Disp();
<<EOF>> { drv.noop(); return parser::make_END_OF_FILE(); }
