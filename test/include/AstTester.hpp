#include "ASTNode.hpp"

#include <vector>
#include <initializer_list>
#include <variant>
#include <string>
#include <optional>

enum class Node{
    InstructionNode,
    BinaryExpNode,
    FlowControl,
    Literal,
    VariableNode
};

//using optionalData = std::optional<std::variant<Instructions, std::string>>;
using optionalData = std::tuple<std::optional<Instructions>, std::optional<std::string>>;

class AstTestWalker : public ASTWalker {
private:
    std::vector<std::pair<Node, optionalData>> toWalk;
    int index;
    optionalData CheckNode(Node);
public:
    AstTestWalker(
        std::initializer_list<std::pair<Node, optionalData>>);

    void RunTest(std::string input);

    void WalkNode(InstructionNode*) override;
    void WalkNode(FlowControl*) override;
    void WalkNode(BinaryExpNode*) override;
    void WalkNode(VariableNode*) override;
    void WalkNode(LiteralNode*) override;
};