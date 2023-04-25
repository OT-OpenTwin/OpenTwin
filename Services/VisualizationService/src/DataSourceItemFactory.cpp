#include "DataSourceItemFactory.h"

#include "DataSourceManagerItem.h"
#include "EntityBase.h"

#include "DataSourceCartesianVector.h"
#include "DataSourceResult3D.h"

DataSourceManagerItem *DataSourceItemFactory::createSourceItem(EntityBase *resultEntity)
{
	if (resultEntity->getClassName() == "EntityCartesianVector")
	{
		return new DataSourceCartesianVector;
	}
	else if (resultEntity->getClassName() == "EntityResult3DData")
	{
		return new DataSourceResult3D;
	}
	else
	{
		assert(0); // Unknown data type
	}

	return nullptr;
}

