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

    walker.RunTest(R"(If 1 = 1
        Disp "Good"
        End)");

    walker.RunTest(R"(If (1 = 1)
        THEN
        Disp "Good
        End)");

    walker.RunTest(R"(If (1 = 1): Then
        Disp "Good
        End)");

    walker.RunTest(R"(If (1 = 1): Then
        Disp "Good
        End)");
}

TEST_CASE("Assign") {
    AstTestWalker walker{ {Node::Literal, {}},
                          {Node::BinaryExpNode, {Instructions::Equals, {}}},
                          {Node::Literal, {}},
                          {Node::BinaryExpNode, {Instructions::Assign, {}}},
                          {Node::VariableNode, {}} };
    walker.RunTest(R"((1 = 1) -> A)");
}