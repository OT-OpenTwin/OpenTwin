#include "ClassFactoryBlock.h"
#include "EntityBlock.h"
#include "EntityBlockDatabaseAccess.h"
#include "EntityBlockPlot1D.h"
#include "EntityBlockPython.h"
#include <cassert>

EntityBase* ClassFactoryBlock::CreateEntity(const std::string& entityType)
{
	if (entityType == "EntityBlock")
	{
		return new EntityBlock(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockPlot1D")
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
	
	return ClassFactoryHandlerAbstract::CreateEntity(entityType);
}
