#include "ASTNode.hpp"

#include <vector>
#include <initializer_list>
#include <variant>
#include <string>
#include <optional>


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

    // Inherited via ASTWalker
    void WalkNode(LblNode*) override;
    void WalkNode(GotoNode*) override;
    void WalkNode(InstructionNode*) override;
    void WalkNode(FlowControl*) override;
    void WalkNode(BinaryExpNode*) override;
    void WalkNode(VariableNode*) override;
    void WalkNode(LiteralNode*) override;
    void WalkNode(BasicJump*) override;
};