// @otlicense
// File: MongoWrapper.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"
#include "OTServiceFoundation/UserCredentials.h"
#include "OTDataStorage/Connection/ConnectionAPI.h"
#include "OTSystem/AppExitCodes.h"
#include "OTModelEntities/DataBase.h"
#include "OTDataStorage/Document/DocumentAccess.h"
#include "OTDataStorage/Helper/QueryBuilder.h"
#include "OTDataStorage/Helper/BsonValuesHelper.h"


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


using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using BsonViewOrValue = bsoncxx::document::view_or_value;


// std header
#include <string>
#include <mutex>
#include <optional>
#include <algorithm>

class MongoWrapper {
public:
	MongoWrapper(std::string _siteID);
	~MongoWrapper() = default;

	std::string getDocumentList(const ot::LibraryElementSelectionCfg& _selectionCfg, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);
	std::string getCompleteDocument(const std::string& _collectionName, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, const std::string& _selectedDocument);
	void migrateLibraryEntryDataToGridFS(const std::string& _collectionName, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, const std::string& _selectedDocument);
private:
	// Database connection and validation
	std::string getMongoURL(std::string _databaseURL, std::string _dbUserName, std::string _dbPassword);
	bool initializeConnection(const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);
	bool checkCollectionExists(const std::string& _collectionName);

	// Document query helpers
	bsoncxx::document::value buildCombinedFilterQuery(const std::list<std::pair<std::string, std::string>>& _additionalFilters, const std::list<std::pair<std::string, std::string>>& _metadataFilters);
	bsoncxx::stdx::optional<bsoncxx::document::value> fetchDocumentByName(DataStorageAPI::DocumentAccessBase& _docBase, const std::string& _documentName);

	// Data loading helpers
	std::string loadDocumentData(const bsoncxx::document::view& _documentView, const std::string& _collectionName);
	std::string loadGridFSData(const bsoncxx::oid& _oid, const std::string& _collectionName);

	std::string m_databaseURL;
	std::string m_siteID;
	std::string dbName = "Libraries";
};