#include "ASTNode.hpp"

#include <vector>
#include <initializer_list>
#include <variant>
#include <string>

enum class Node{
    InstructionNode,
    BinaryExpNode,
    FlowControl
};

class AstTestWalker : public ASTWalker {
private:
    std::vector<std::pair<Node, std::variant<Instructions, std::string>>> toWalk;
    int index;
public:
    AstTestWalker(
        std::initializer_list<std::pair<Node, std::variant<Instructions, std::string>>>);

    void RunTest(std::string input);

    void WalkNode(InstructionNode*) override;
    void WalkNode(FlowControl*) override;
    void WalkNode(BinaryExpNode*) override;
};