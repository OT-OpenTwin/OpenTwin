#pragma once

#include <string>

class EntityBase;
class ClassFactory;

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

	virtual bool loadData(EntityBase *resultEntity, EntityBase *meshEntity, ClassFactory *classFactory) = 0;

private:
};
