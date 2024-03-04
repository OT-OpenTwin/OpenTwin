#include <cassert>

#include "ClassFactoryBlock.h"
#include "EntityBlock.h"
#include "EntityBlockDatabaseAccess.h"
#include "EntityBlockPlot1D.h"
#include "EntityBlockPython.h"
#include "EntityBlockCircuitElement.h"
#include "EntityBlockCircuitResistor.h"
#include "EntityBlockDataDimensionReducer.h"
#include "EntityBlockStorage.h"
#include "EntityBlockConnection.h"

EntityBase* ClassFactoryBlock::CreateEntity(const std::string& entityType)
{
	if (entityType == "EntityBlockPlot1D")
	{
		return new EntityBlockPlot1D(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockDatabaseAccess")
	{
		return new EntityBlockDatabaseAccess(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockPython")
	{
		return new EntityBlockPython(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockCircuitElement")
	{
		return new EntityBlockCircuitElement(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockCircuitResistor")
	{
		return new EntityBlockCircuitResistor(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockDataDimensionReducer")
	{
		return new EntityBlockDataDimensionReducer(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockStorage")
	{
		return new EntityBlockStorage(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockConnection")
	{
		return new EntityBlockConnection(0, nullptr, nullptr, nullptr, this, "");
	}
	
	return ClassFactoryHandlerAbstract::CreateEntity(entityType);
}
