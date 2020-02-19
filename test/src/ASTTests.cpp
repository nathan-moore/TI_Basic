#include "AstTester.hpp"
#include "catch.hpp"

TEST_CASE( "PARSE_DISP" ) {
    AstTestWalker walker{{Node::InstructionNode, Instructions::Disp},
                         {Node::InstructionNode, Instructions::Disp}};

    walker.RunTest(std::string("Disp \"Just Saying Hello\n")+
                               "Disp \"Just Saying goodbye\n");

}