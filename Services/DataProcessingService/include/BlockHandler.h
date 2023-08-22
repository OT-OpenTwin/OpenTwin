#pragma once
#include <list>
#include "OpenTwinCore/Variable.h"
#include "EntityBlock.h"

class BlockHandler
{
public:
	using genericDataBlock = std::list<ot::variable_t>;

	virtual genericDataBlock Execute(genericDataBlock& inputData) = 0;

};
