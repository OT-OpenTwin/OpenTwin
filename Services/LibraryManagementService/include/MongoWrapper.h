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
#include "OTCore/LogDispatcher.h"
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

	std::string getDocument(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);
	std::string getDocumentList(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);
	std::string getMetaData(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);
private:
	std::string getMongoURL(std::string _databaseURL, std::string _dbUserName, std::string _dbPassword);

	std::string m_databaseURL;
	std::string m_siteID;
	std::string dbName = "Libraries";



};