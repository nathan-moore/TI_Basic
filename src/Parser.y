%require "3.5"
%language "c++"
%define api.value.type variant
%define api.token.constructor

%code requires {
    #include <string>
    #include <memory>
    #include "ASTNode.hpp"
}

%{
    #include "ASTNode.hpp"
    #include "Parser.tab.hh"

    yy::parser::symbol_type yylex(void);
%}

%token T_If T_Equals T_Not_Equals T_Then T_Else T_Done T_End
%token T_Disp
%token <char*> T_String

%token END_OF_FILE 0

%type <std::string> Displayable;
%type <std::shared_ptr<AstNode>> command C_Disp if_stmt statement;
%type <std::shared_ptr<InstructionList>> instruction_list else_part;
%type <std::shared_ptr<ExpNode>> exp;

%%

program: 
    instruction_list
    {

    };

instruction_list:
    statement instruction_list
    {
        $2->push_back($1);
        $$ = $2;
    }
    | %empty
    {
        $$ = std::make_shared<InstructionList>();
    };

statement:
    command { $$ = std::move($1); }
    | if_stmt { $$ = std::move($1); };

if_stmt:
    T_If exp T_Then instruction_list else_part T_End
    {
        $$ = std::make_shared<FlowControl>($2, $4, $5);
    };

else_part:
    T_Else instruction_list
    {
        $$ = $2;
    }
    | %empty
    {
        $$ = std::shared_ptr<InstructionList>();
    };

exp:
    %empty
    {};

command:
    C_Disp { $$ = $1; };

C_Disp:
    T_Disp Displayable
    {
        $$ = std::make_shared<InstructionNode>(Instructions::Disp, $2);
    };

Displayable:
    T_String
    {
        $$ = std::string($1);
    };