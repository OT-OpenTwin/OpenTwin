#pragma once
#include "OTDataStorage/DataStorageAPIExport.h"
#include "OTCore/QueryDescription/DataLakeAccessCfg.h"
#include <mongocxx/options/find.hpp>
#include "OTDataStorage/DataLakeAPI.h"
#include <vector>


class DATASTORAGE_API DataLakeHelper
{
public:
	static ot::JsonDocument executeQuery(const ot::DataLakeAccessCfg& _config, mongocxx::options::find _options, std::string& _log);
	static std::string getTransformedCollectionEnding() { return ".transformed"; }
	static std::string getResultCollectionEnding() { return ".results"; }
	static void createDefaultIndexes(DataStorageAPI::DataLakeAPI& _dataLakeAPI);
	static void createDefaultIndexes(const std::string& _collectionName);
	static const std::string& getDataFieldName() { return m_resultDataField; }
private:
	static const std::string m_resultDataField;
	
	static ot::JsonDocument createClearTextResult(const ot::DataLakeAccessCfg& _config, const ot::JsonDocument& _databaseResults, std::string& _log);
};
