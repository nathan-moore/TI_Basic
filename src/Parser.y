%require "3.5"
%language "c++"
%define api.value.type variant
%define api.token.constructor
%define parse.trace

%code requires {
    #include <string>
    #include <memory>
    #include "ASTNode.hpp"
    class driver;
}

%param {driver& drv }

%{
    #include "ASTNode.hpp"
    #include "Parser.tab.hh"
    #include "Driver.hh"
%}

%token T_If T_Equals T_Not_Equals T_Then T_Else T_Done T_End T_Double_Equals T_Right_Paren T_Left_Paren
%token T_Disp T_Plus T_Star
%token <char*> T_String
%token <int> T_Int
%token <float> T_Float

%token END_OF_FILE 0

%type <std::string> Displayable;
%type <std::shared_ptr<AstNode>> command C_Disp if_stmt statement;
%type <std::shared_ptr<InstructionList>> instruction_list else_part;
%type <std::shared_ptr<ExpNode>> exp primary;

%%

program: 
    instruction_list
    {
        drv.topNode = $1;
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
    }
    | T_If exp T_Then instruction_list
    {
        $$ = std::make_shared<FlowControl>($2, $4, std::shared_ptr<InstructionList>(nullptr));
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
    exp_prefix factor
    {
    };

exp_prefix:
    exp_prefix factor addop
    {
    }
    | %empty
    {
        $$ = nullptr;
    };

factor:
    factor_prefix postfix_exp
    {
    };

factor_prefix:
    factor_prefix postfix_exp mulop
    {}
    | %empty 
    {};

postfix_exp:
    primary
    {};

primary:
    T_Left_Paren exp T_Right_Paren
    {}
    | id
    {};

addop:
    T_Plus
    {};

mulop:
    T_Star
    {};

id:
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