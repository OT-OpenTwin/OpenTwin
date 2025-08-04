#include "DataSourceItemFactory.h"

#include "DataSourceManagerItem.h"
#include "EntityBase.h"

#include "DataSourceCartesianVector.h"
#include "DataSourceResult3D.h"
#include "DataSourceUnstructuredMesh.h"

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
	else if (resultEntity->getClassName() == "EntityResultUnstructuredMeshData")
	{
		return new DataSourceUnstructuredMesh;
	}
	else if (resultEntity->getClassName() == "EntityResultUnstructuredMeshVtk")
	{
		return new DataSourceUnstructuredMesh;
	}
	else
	{
		assert(0); // Unknown data type
	}

	return nullptr;
}

