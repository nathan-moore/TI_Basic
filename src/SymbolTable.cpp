#include "SymbolTable.hpp"


Variable* SymbolTable::AddVariableUse(const std::string& str, std::weak_ptr<VariableNode> var)
{
	if (symbols.count(str) != 0)
	{
		symbols.insert({ str, {} });
	}

	symbols[str].references.push_back(var);

	return &symbols[str];
}
