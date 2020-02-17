%{
    #include "Parser.tab.hh"
    #include "Driver.hh"

    using namespace yy;
%}

%option noyywrap nounput noinput batch debug


%%

"=" return parser::make_T_Equals();
\"[^\"]*\" return parser::make_T_String(yytext);
