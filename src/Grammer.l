%{
    #include "Driver.hh"
    #include "Parser.tab.hh"

    #include <string>

    using namespace yy;
%}

%option noyywrap nounput noinput batch


%%

"==" return parser::make_T_Double_Equals();
\"[^\"\n]*[\"\n] return parser::make_T_String(yytext);
"Disp " return parser::make_T_Disp();
<<EOF>> { drv.noop(); return parser::make_END_OF_FILE(); }
"If" return parser::make_T_If();
[0-9]+ return parser::make_T_Int(std::stoi(yytext));
 "(" return parser::make_T_Left_Paren();
 ")" return parser::make_T_Right_Paren();
 (?i:then) return parser::make_T_Then();
 [ \n] ;

