// @otlicense

#pragma once

#include <string>

#include "DataSourceManagerItem.h"

class EntityCartesianVector;
class EntityMeshCartesianData;

class DataSourceCartesianVector : public DataSourceManagerItem
{
public:
	DataSourceCartesianVector();
	virtual ~DataSourceCartesianVector();

	virtual bool loadData(EntityBase *resultEntity, EntityBase *meshEntity) override;

	EntityCartesianVector *getResultData(void) { return resultData; }
	EntityMeshCartesianData *getMeshData(void) { return meshData; }

private:
	EntityCartesianVector *resultData;
	EntityMeshCartesianData *meshData;
};
