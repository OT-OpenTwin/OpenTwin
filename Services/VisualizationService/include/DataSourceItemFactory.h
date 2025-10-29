// @otlicense

#pragma once

class DataSourceManagerItem;
class EntityBase;

class DataSourceItemFactory
{
public:
	static DataSourceManagerItem *createSourceItem(EntityBase *resultEntity);

};

