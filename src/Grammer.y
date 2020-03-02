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

%token T_If T_Equals T_Not_Equals T_Then T_Else T_Done T_End T_Compare_Equals T_Right_Paren T_Left_Paren
%token T_Disp T_Plus T_Star T_Newline T_Colon T_Assign
%token <char*> T_String T_Identifier
%token <int> T_Int
%token <float> T_Float

%token END_OF_FILE 0

%type <std::string> Displayable;
%type <std::shared_ptr<AstNode>> command C_Disp if_stmt statement assign_statement;
%type <std::unique_ptr<InstructionList>> instruction_list;
%type <std::shared_ptr<ExpNode>> exp primary if_then_part base_exp factor postfix_exp id;
%type <BinaryExpNodeBuilder> exp_prefix factor_prefix;
%type <Instructions> mulop addop

%%

program: 
    instruction_list
    {
        drv.topNode = std::move($1);
    };

instruction_list:
    statement instruction_list
    {
        $2->push_back($1);
        $$ = std::move($2);
    }
    | %empty
    {
        $$ = std::make_unique<InstructionList>();
    };

statement:
    command T_Newline { $$ = std::move($1); }
    | if_stmt { $$ = std::move($1); }
    | assign_statement {$$ = std::move($1);}

assign_statement:
    exp T_Assign id
    {
        $$ =  std::make_shared<BinaryExpNode>(Instructions::Assign, $1, $3);
    };

if_stmt:
    if_then_part instruction_list T_Else instruction_list T_End
    {
        $$ = std::make_shared<FlowControl>($1, std::move($2), std::move($4));
    }
    | if_then_part instruction_list T_End
    {
        $$ = std::make_shared<FlowControl>($1, std::move($2));
    };

if_then_part:
    T_If exp Ending T_Then T_Newline
    {
        $$ = $2;
    }
    | T_If exp T_Newline
    {
        $$ = $2;
    };

exp:
    base_exp
    {
        $$ = $1;
    }
    //This forbids 1 = 1 = 1 = 1 You could argue that it should be legal, but
    //it's both ugly and easier to forbid
    // 1 == (1 == 1) is legal though
    | base_exp T_Compare_Equals base_exp
    {
        $$ = std::make_shared<BinaryExpNode>(Instructions::Equals, $1, $3);
    };

base_exp:
    exp_prefix factor
    {
        if ($1.IsDefault())
            $$ = $2;
        else 
            $$ = $1.Build($2);
    };

exp_prefix:
    exp_prefix factor addop
    {
        if ($1.IsDefault())
            $$ = BinaryExpNodeBuilder($3, $2);
        else
            $$ = BinaryExpNodeBuilder($3, $1.Build($2));
    }
    | %empty
    {
        $$ = BinaryExpNodeBuilder();
    };

factor:
    factor_prefix postfix_exp
    {
        if ($1.IsDefault())
            $$ = $2;
        else
            $1.Build($2);
    };

factor_prefix:
    factor_prefix postfix_exp mulop
    {
        if ($1.IsDefault())
             $$ = BinaryExpNodeBuilder($3, $2);
        else
             $$ = BinaryExpNodeBuilder($3, $1.Build($2));
    }
    | %empty 
    {
         $$ = BinaryExpNodeBuilder();
    };

postfix_exp:
    primary
    {
        $$ = $1;
    };

primary:
    T_Left_Paren exp T_Right_Paren
    {
        $$ = $2;
    }
    | id
    {
        $$ = $1;
    }
    | T_Int
    {
        $$ = std::make_shared<LiteralNode>($1);
    }
    | T_Float
    {
        $$ = std::make_shared<LiteralNode>($1);
    };

addop:
    T_Plus
    {
        $$ = Instructions::Add;
    };

mulop:
    T_Star
    {
        $$ = Instructions::Multiply;
    };

id:
     T_Identifier 
     {
        //TODO
        $$ = std::make_shared<VariableNode>();
     };

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