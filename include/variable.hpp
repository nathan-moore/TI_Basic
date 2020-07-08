#ifndef Variable__
#define Variable__

#include "ASTNode.hpp"

#include <vector>

class Variable {
	//Whatever needs to be here
	friend class SymbolTable;
	std::vector<std::weak_ptr<VariableNode>> references;
	uint32_t SSACount = 0;
public:
	uint32_t GetCount()
	{
		return SSACount++;
	}
};

#endif