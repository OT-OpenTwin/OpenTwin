
#include "stdafx.h"

#include "DataSourceManager.h"
#include "DataSourceManagerItem.h"
#include "DataSourceItemFactory.h"
#include "EntityBase.h"
#include "ClassFactory.h"

#include "EntityAPI.h"

std::map<std::pair<ot::UID, ot::UID>, DataSourceManagerItem *> DataSourceManager::dataItems;

DataSourceManager::DataSourceManager()
{

}

DataSourceManager::~DataSourceManager()
{
	// We do not delete the data items here, since we keep them persistent until they are explicitly deleted.
}

DataSourceManagerItem *DataSourceManager::getDataItem(ot::UID sourceID, ot::UID sourceVersion, ot::UID meshID, ot::UID meshVersion, ot::components::ModelComponent *modelComponent, ClassFactory *classFactory)
{
	if (!isDataItemLoaded(sourceID, sourceVersion))
	{
		// Now load the data item to determine its type
		EntityBase *resultEntity = dynamic_cast<EntityBase*>(ot::EntityAPI::readEntityFromEntityIDandVersion(sourceID, sourceVersion, *classFactory));
		EntityBase *meshEntity = nullptr;

		if (meshID > 0)
		{
			meshEntity = dynamic_cast<EntityBase*>(ot::EntityAPI::readEntityFromEntityIDandVersion(meshID, meshVersion, *classFactory));
		}

		if (resultEntity != nullptr)
		{
			DataSourceManagerItem *newItem = DataSourceItemFactory::createSourceItem(resultEntity);

			if (newItem->loadData(resultEntity, meshEntity, classFactory))
			{
				dataItems[std::pair<ot::UID, ot::UID>(sourceID, sourceVersion)] = newItem;
			}
			else
			{
				delete newItem;
			}
		}
	}

	// Now we check whether the item is available (and return nullptr if not).
	if (!isDataItemLoaded(sourceID, sourceVersion)) return nullptr;
	return dataItems[std::pair<ot::UID, ot::UID>(sourceID, sourceVersion)];
}

void DataSourceManager::deleteDataItem(ot::UID sourceID, ot::UID sourceVersion)
{
	if (isDataItemLoaded(sourceID, sourceVersion))
	{
		delete dataItems[std::pair<ot::UID, ot::UID>(sourceID, sourceVersion)];
		dataItems.erase(std::pair<ot::UID, ot::UID>(sourceID, sourceVersion));
	}
}

bool DataSourceManager::isDataItemLoaded(ot::UID sourceID, ot::UID sourceVersion)
{
	return (dataItems.count(std::pair<ot::UID, ot::UID>(sourceID, sourceVersion)) != 0);
}

void DataSourceManager::deleteAllDataItems(void)
{
	for (auto item : dataItems)
	{
		delete item.second;
	}

	dataItems.clear();
}
