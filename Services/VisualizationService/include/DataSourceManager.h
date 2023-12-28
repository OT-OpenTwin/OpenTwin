#pragma once

class DataSourceManagerItem;

#include <string>
#include <map>

#include "Types.h"

#include "OTServiceFoundation/ModelComponent.h"

class ClassFactory;

class DataSourceManager
{
public:
	DataSourceManager();
	virtual ~DataSourceManager();

	static DataSourceManagerItem *getDataItem(ot::UID sourceID, ot::UID sourceVersion, ot::UID meshID, ot::UID meshVersion, ot::components::ModelComponent *modelComponent, ClassFactory* classFactory);

	static void deleteDataItem(ot::UID sourceID, ot::UID sourceVersion);
	static bool isDataItemLoaded(ot::UID sourceID, ot::UID sourceVersion);

	static void deleteAllDataItems(void);

private:
	static std::map<std::pair<ot::UID, ot::UID>, DataSourceManagerItem *> dataItems;
};
