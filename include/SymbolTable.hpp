#ifndef SymbolTable__
#define SymbolTable__

#include "variable.hpp"
#include "ASTNode.hpp"

#include <unordered_map>
#include <memory>

class SymbolTable {
	std::unordered_map<std::string, Variable> symbols;
public:
	Variable* AddVariableUse(const std::string& str, std::weak_ptr<VariableNode> var);
};

#endif