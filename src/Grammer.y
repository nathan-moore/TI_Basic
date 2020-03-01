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
    #include "Grammer.tab.hh"
    #include "Driver.hh"
%}

%token T_If T_Equals T_Not_Equals T_Then T_Else T_Done T_End T_Double_Equals T_Right_Paren T_Left_Paren
%token T_Disp T_Plus T_Star T_Newline T_Colon
%token <char*> T_String T_Identifier
%token <int> T_Int
%token <float> T_Float

%token END_OF_FILE 0

%type <std::string> Displayable;
%type <std::shared_ptr<AstNode>> command C_Disp if_stmt statement;
%type <std::shared_ptr<InstructionList>> instruction_list;
%type <std::shared_ptr<ExpNode>> exp primary if_then_part;

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
    command T_Newline { $$ = std::move($1); }
    | if_stmt { $$ = std::move($1); };

if_stmt:
    if_then_part instruction_list T_Else instruction_list T_End
    {
        $$ = std::make_shared<FlowControl>($1, $2, $4);
    }
    | if_then_part instruction_list T_End
    {
        $$ = std::make_shared<FlowControl>($1, $2, std::shared_ptr<InstructionList>(nullptr));
    };

if_then_part:
    T_If exp Ending T_Then T_Newline
    {
        $$ = $2;
    }

exp:
    base_exp
    {
    }
    //This forbids 1 == 1 == 1 == 1 You could argue that it should be legal, but
    //t's both ugly and easier to forbid
    | base_exp T_Double_Equals base_exp
    {};

base_exp:
    exp_prefix factor
    {};

exp_prefix:
    exp_prefix factor addop
    {
    }
    | %empty
    {};

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
    {}
    | T_Int
    {}
    | T_Float
    {};

addop:
    T_Plus
    {};

mulop:
    T_Star
    {};

id:
     T_Identifier {};

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

Ending:
    T_Newline | T_Colon