//! @file Database.h
//! @author Sebastian Urmann
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTServiceFoundation/UserCredentials.h"
#include "Connection/ConnectionAPI.h"
#include "OTSystem/AppExitCodes.h"
#include "DataBase.h"
#include "Document/DocumentAccess.h"
#include "Helper\QueryBuilder.h"
#include "Helper\BsonValuesHelper.h"


// ThirdParty header
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>





// std header
#include <string>
#include <mutex>
#include <optional>
#include <algorithm>

class MongoWrapper {
public:
	MongoWrapper(std::string _siteID);
	~MongoWrapper() = default;

	std::optional<bsoncxx::document::value> getDocument(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value);
	std::string getDocumentList(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);

private:
	std::string getMongoURL(std::string _databaseURL, std::string _dbUserName, std::string _dbPassword);

	std::string m_databaseURL;
	std::string m_siteID;
	std::string dbName = "Libraries";



};