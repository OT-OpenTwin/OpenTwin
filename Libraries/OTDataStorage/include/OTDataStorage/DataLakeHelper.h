#pragma once
#include "OTDataStorage/DataStorageAPIExport.h"
#include "OTCore/QueryDescription/DataLakeAccessCfg.h"
#include <mongocxx/options/find.hpp>
#include "OTDataStorage/DataLakeAPI.h"
#include <vector>


class DATASTORAGE_API DataLakeHelper
{
public:
	static ot::JsonDocument executeQuery(ot::DataLakeAccessCfg& _config, mongocxx::options::find _options);
	static std::string getTransformedCollectionEnding() { return ".transformed"; }
	static std::string getResultCollectionEnding() { return ".results"; }
	static void createDefaultIndexes(const std::string& _collectionName, DataStorageAPI::DataLakeAPI& _dataLakeAPI);
private:
	static const std::string m_resultDataField;
	
	static ot::JsonDocument createClearTextResult(ot::DataLakeAccessCfg& _config, const ot::JsonDocument& _databaseResults);
};
