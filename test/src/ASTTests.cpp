#include "AstTester.hpp"
#include "catch.hpp"

TEST_CASE( "PARSE_DISP" ) {
    AstTestWalker walker{ {Node::InstructionNode, {Instructions::Disp, {}}},
                          {Node::InstructionNode, {Instructions::Disp, {}}} };

    walker.RunTest(std::string("Disp \"Just Saying Hello\n")+
                               "Disp \"Just Saying goodbye\n");

    walker.RunTest(std::string("Disp \"Just Saying Hello\"\n") +
                               "Disp \"Hello\"\n");
}

TEST_CASE("PARSE_IF") {
    AstTestWalker walker{ {Node::FlowControl, {}},
                          {Node::Literal, {}},
                          {Node::BinaryExpNode, {}},
                          {Node::Literal, {}},
                          {Node::InstructionNode, {Instructions::Disp, "Good"}} };

    //TODO:
    //walker.RunTest(std::string("If 1 == 1\n") +
    //    "Disp \"Good\"\n");

    walker.RunTest(std::string("If (1 == 1)\n") +
        "THEN\n"
        "Disp \"Good\n" +
        "End");

    walker.RunTest(std::string("If (1 == 1): Then") +
        "Disp \"Good\n" +
        "End");

    walker.RunTest(std::string("If (1 == 1): Then") +
        "Disp \"Good" +
        "End");
}