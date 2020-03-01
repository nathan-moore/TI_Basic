%{
    #include "Driver.hh"
    #include "Grammer.tab.hh"

    #include <string>

    using namespace yy;
%}

%option noyywrap nounput noinput batch


%%

"==" return parser::make_T_Double_Equals();
"Disp " return parser::make_T_Disp();
<<EOF>> { drv.noop(); return parser::make_END_OF_FILE(); }
"If" return parser::make_T_If();
[0-9]+ return parser::make_T_Int(std::stoi(yytext));
[0-9]+([.])[0-9]+ return parser::make_T_Float(std::stof(yytext));
"(" return parser::make_T_Left_Paren();
")" return parser::make_T_Right_Paren();
(?i:then) return parser::make_T_Then();
(?i:else) return parser::make_T_Else();
(?i:end) return parser::make_T_End();
"+" return parser::make_T_Plus();
"*" return parser::make_T_Star();
[a-zA-Z_][a-zA-Z0-9_]* return parser::make_T_Identifier(yytext);
\n return parser::make_T_Newline();
: return parser::make_T_Colon();
\"[^\"\n]*\"? return parser::make_T_String(yytext);

[ ] ;

