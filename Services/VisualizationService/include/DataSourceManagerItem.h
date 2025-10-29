// @otlicense

#pragma once

#include <string>

class EntityBase;

namespace ot
{
	namespace components
	{
		class ModelComponent;
	}
}

class DataSourceManagerItem
{
public:
	DataSourceManagerItem();
	virtual ~DataSourceManagerItem();

	virtual bool loadData(EntityBase *resultEntity, EntityBase *meshEntity) = 0;

private:
};
