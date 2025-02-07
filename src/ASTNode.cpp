#include "ASTNode.hpp"
#include "BasicBlock.hpp"
#include "SSAFormer.hpp"

#include <assert.h>
#include <iostream>

InstructionNode::InstructionNode(Instructions i, const DisplayHelper& s)
    :     items(s), 
    instruction(i)
{}

void InstructionNode::PreOrderWalk(ASTWalker * walker)
{
    walker->WalkNode(this);

    if (instruction == Instructions::Disp)
    {
        DisplayHelper helper = std::get<DisplayHelper>(items);
        auto lamda = [=](std::shared_ptr<VariableNode>& node) {node->PreOrderWalk(walker); };
        helper.VistNodes(lamda);
    }
}

void InstructionNode::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
 
    if (instruction == Instructions::Disp)
    {
        DisplayHelper helper = std::get<DisplayHelper>(items);
        helper.VistNodes([=](std::shared_ptr<VariableNode>& node) {node->InOrderWalk(walker); });
    }
}

void InstructionNode::PostOrderWalk(ASTWalker* walker)
{
    if (instruction == Instructions::Disp)
    {
        DisplayHelper helper = std::get<DisplayHelper>(items);
        helper.VistNodes([=](std::shared_ptr<VariableNode>& node) {node->PostOrderWalk(walker); });
    }

    walker->WalkNode(this);
}

InstructionList::InstructionList() {}

void InstructionList::PreOrderWalk(ASTWalker* walker)
{
    walker->WalkList(this);
    for (std::shared_ptr<AstNode> node : list)
    {
        walker->TopLevelNode();
        node->PreOrderWalk(walker);
    }
}

void InstructionList::InOrderWalk(ASTWalker* walker)
{
    walker->WalkList(this);
    for (std::shared_ptr<AstNode> node : list)
    {
        walker->TopLevelNode();
        node->InOrderWalk(walker);
    }
}

void InstructionList::PostOrderWalk(ASTWalker* walker)
{
    for (std::shared_ptr<AstNode> node : list)
    {
        node->PostOrderWalk(walker);
        walker->TopLevelNode();
    }
    walker->WalkList(this);
}

void InstructionList::push_front(std::shared_ptr<AstNode> node)
{
    list.insert(list.begin(), node);
}

void InstructionList::push_back(std::shared_ptr<AstNode> node)
{
    list.push_back(node);
}

BinaryExpNode::BinaryExpNode(Instructions i, 
    std::shared_ptr<ExpNode> left,
    std::shared_ptr<ExpNode> right)
    : leftNode(left),
    rightNode(right),
    token(i)
{}

void BinaryExpNode::PreOrderWalk(ASTWalker * walker)
{
    walker->WalkNode(this);
    leftNode->PreOrderWalk(walker);
    rightNode->PreOrderWalk(walker);
}

void BinaryExpNode::InOrderWalk(ASTWalker* walker)
{
    leftNode->InOrderWalk(walker);
    walker->WalkNode(this);
    rightNode->InOrderWalk(walker);
}

void BinaryExpNode::PostOrderWalk(ASTWalker* walker)
{
    leftNode->PostOrderWalk(walker);
    rightNode->PostOrderWalk(walker);

    walker->WalkNode(this);
}

std::shared_ptr<ExpNode>& FlowControl::getCond()
{
    return condition;
}

std::unique_ptr<InstructionList>& FlowControl::getIfList()
{
    return ifList;
}

std::unique_ptr<InstructionList>& FlowControl::getElseList()
{
    return elseList;
}

FlowControl::FlowControl(std::shared_ptr<ExpNode> exp,
    std::unique_ptr<InstructionList> ifBranch,
    std::unique_ptr<InstructionList> elseBranch)
    : condition(exp),
    ifList(std::move(ifBranch)),
    elseList(std::move(elseBranch))
{}

void FlowControl::PreOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
    condition->PreOrderWalk(walker);
    getIfList()->PreOrderWalk(walker);
    std::unique_ptr<InstructionList>& elses = getElseList();
    if (elses != nullptr)
    {
        elses->PreOrderWalk(walker);
    }
}

void FlowControl::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
    condition->InOrderWalk(walker);
    getIfList()->InOrderWalk(walker);
    std::unique_ptr<InstructionList>& elses = getElseList();
    if (elses != nullptr)
    {
        elses->InOrderWalk(walker);
    }
}

void FlowControl::PostOrderWalk(ASTWalker* walker)
{
    condition->PostOrderWalk(walker);
    getIfList()->PostOrderWalk(walker);
    std::unique_ptr<InstructionList>& elses = getElseList();
    if (elses != nullptr)
    {
        elses->PostOrderWalk(walker);
    }

    walker->WalkNode(this);
}

void LiteralNode::PreOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void LiteralNode::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void LiteralNode::PostOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void VariableNode::PreOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void VariableNode::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void VariableNode::PostOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void VariableNode::SetExpType(Type t)
{
    assert(ssaVariable != nullptr);
    ssaVariable->t = t;
}

Type VariableNode::GetExpType()
{
    if (ssaVariable == nullptr)
        return Type::Unknown;

    return ssaVariable->t;
}

void LblNode::PreOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void LblNode::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void LblNode::PostOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void GotoNode::PreOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void GotoNode::InOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

void GotoNode::PostOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
}

bool GotoNode::HasBB()
{
    return std::holds_alternative<std::shared_ptr<LblNode>>(destination);
}

BasicBlock* GotoNode::GetBB()
{
    assert(HasBB());
    return std::get<std::shared_ptr<LblNode>>(destination)->GetBB();
}

std::string& GotoNode::GetStr()
{
    assert(!HasBB());
    return std::get<std::string>(destination);
}

void BasicJump::PreOrderWalk(ASTWalker* walker)
{
    walker->WalkNode(this);
    condition->PreOrderWalk(walker);
    ifJmp->PreOrderWalk(walker);

    if (elseJmp != nullptr)
        elseJmp->PreOrderWalk(walker);
}

void BasicJump::InOrderWalk(ASTWalker* walker)
{
    condition->InOrderWalk(walker);
    walker->WalkNode(this);
    ifJmp->InOrderWalk(walker);
    
    if (elseJmp != nullptr)
        elseJmp->InOrderWalk(walker);
}

void BasicJump::PostOrderWalk(ASTWalker* walker)
{
    condition->PostOrderWalk(walker);
    walker->WalkNode(this);

    if (elseJmp != nullptr)
        elseJmp->PostOrderWalk(walker);

    ifJmp->PostOrderWalk(walker);
}

std::string InstToString(Instructions node)
{
    std::string inst;

    switch (node)
    {
    case Instructions::Disp:
        inst = "Disp";
        break;
    case Instructions::Equals:
        inst = "Equals";
        break;
    case Instructions::Nothing:
        inst = "Nothing";
        break;
    case Instructions::Add:
        inst = "Add";
        break;
    case Instructions::Multiply:
        inst = "Multiply";
        break;
    case Instructions::Assign:
        inst = "Assign";
        break;
    }

    return inst;
}

std::string TypeToString(Type t)
{
    std::string str;

    switch (t)
    {
    case Type::Float:
        str = "Float";
        break;
    case Type::Integer:
        str = "Integer";
        break;
    case Type::Dynamic:
        str = "Dynamic";
        break;
    case Type::Unknown:
        str = "Unknown";
        break;
    default:
        str = "Default";
        break;
    }

    return str;
}

std::ostream& operator<<(std::ostream& os, Type t)
{
    return os << TypeToString(t);
}

void ASNodePrinter::WalkNode(InstructionNode* node)
{
    std::cout << "Instruction Node ";


    std::cout << InstToString(node->instruction);

    if (node->instruction == Instructions::Disp)
    {
        std::cout << " " << std::get<0>(node->items).GetPrintableString();
    }

    std::cout << std::endl;
}

void ASNodePrinter::WalkNode(FlowControl*)
{
    std::cout << "FlowControl" << std::endl;
}

void ASNodePrinter::WalkNode(BinaryExpNode* node)
{
    std::cout << "BinaryExpNode: " << InstToString(node->token) << std::endl;
}

void ASNodePrinter::WalkNode(VariableNode* var)
{
    std::cout << "VariableNode: " << var->GetName() << " Type: " << var->GetExpType() << std::endl;
    SSAVariable* ssa = var->GetSSAVariable();
    if (ssa != nullptr)
    {
        std::cout << "SSA num is: " << ssa->VN << std::endl;
    }
}

void ASNodePrinter::WalkNode(LiteralNode* node)
{
    std::cout << "Literal Node Type:" << node->GetExpType() << std::endl;
}

void ASNodePrinter::WalkNode(LblNode* node)
{
    std::cout << "LblNode " << node->GetLbl() << std::endl;
}

void ASNodePrinter::WalkNode(GotoNode* node)
{
    if (!node->HasBB())
    {
        std::cout << "GotoNode " << node->GetStr() << std::endl;
        return;
    }

    BasicBlock* bb = node->GetBB();
    std::cout << "GotoNode " << bb->GetBBNum() << std::endl;
    if (printed.count(bb) == 0)
    {
        toPrint.push(bb);
        printed.insert(bb);
    }
}

void ASNodePrinter::WalkNode(BasicJump*)
{
    std::cout << "BasicJump" << std::endl;
}

void ASNodePrinter::WalkBBs(BasicBlock* firstBB)
{
    toPrint.push(firstBB);
    printed.insert(firstBB);

    while (!toPrint.empty())
    {
        BasicBlock* bb = toPrint.front();
        toPrint.pop();

        std::cout << "Printing bb: " << bb->GetBBNum() << std::endl;
        std::cout << "BBDoms are: ";
        for (BasicBlock* bb : *bb)
        {
            std::cout << bb->GetBBNum() << " ";
        }
        std::cout << std::endl;

        bb->getInstructions()->PreOrderWalk(this);
    
        std::cout << std::endl << std::endl;
    }
}

std::string DisplayHelper::GetPrintableString() const
{
    if (IsTriviableDisplayable())
    {
        return GetTrivialString();
    }
    else
    {
        return GetPrintfFormat();
    }
}

bool DisplayHelper::IsTriviableDisplayable() const
{
    return std::holds_alternative<std::string>(displayable);
}

const std::string& DisplayHelper::GetTrivialString() const
{
    assert(IsTriviableDisplayable());
    return std::get<std::string>(displayable);
}

std::string DisplayHelper::GetPrintfFormat() const
{
    std::string str;

    const std::vector<std::shared_ptr<VariableNode>>& disp = std::get<1>(displayable);
    for (const std::shared_ptr<VariableNode>& node : disp)
    {
        Type t = node->GetExpType();
        if (t == Type::Float)
            str += "%f";
        else if (t == Type::Integer)
            str += "%i";
        else
        {
            assert(false);
            return "";
        }
    }

    return str;
}
