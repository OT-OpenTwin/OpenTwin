// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/QueryDescription/DataLakeAccessCfg.h"
#include "OTDataStorage/DataLakeAPI.h"
#include "OTDataStorage/DataStorageAPIExport.h"

// MongoDB header
#include <mongocxx/options/find.hpp>

// std header
#include <vector>

class OT_DATASTORAGE_API_EXPORT DataLakeHelper
{
public:
	static ot::JsonDocument executeQuery(const ot::DataLakeAccessCfg& _config, mongocxx::options::find _options, std::string& _log);
	static std::string getTransformedCollectionEnding() { return ".transformed"; }
	static std::string getResultCollectionEnding() { return ".results"; }
	static void createDefaultIndexes(DataStorageAPI::DataLakeAPI& _dataLakeAPI);
	static void createDefaultIndexes(const std::string& _collectionName);
	static mongocxx::v_noabi::hint getDefaultIndexHint();
	static const std::string& getDataFieldName() { return m_resultDataField; }
private:
	static const std::string m_resultDataField;
	
	static ot::JsonDocument createClearTextResult(const ot::DataLakeAccessCfg& _config, const ot::JsonDocument& _databaseResults, std::string& _log);
};
