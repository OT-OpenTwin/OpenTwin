// @otlicense
// File: DataLakeAPI.cpp
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

// OpenTwin header
#include "OTDataStorage/DataLakeAPI.h"
#include "OTDataStorage/Connection/ConnectionAPI.h"

// MongoDB header
#include <bsoncxx/json.hpp>

namespace DataStorageAPI
{
	DataLakeAPI::DataLakeAPI(const std::string& _collectionName, const std::string partition)
		:m_documentAccess("Projects", _collectionName + partition), m_docBase("Projects", _collectionName + ".results")
	{	

	}

	DataStorageResponse DataLakeAPI::insertDocumentToDataLakePartition(Document& _jsonData, bool _checkForExistence, bool _allowQueueing)
	{
		auto jsonDataLength = _jsonData.view().length();
		if (jsonDataLength > m_maxDocumentLength)
		{
			throw std::exception(("The result collection does not support documents with a size larger then " + std::to_string(m_maxDocumentLength)).c_str());
		}
		if (_checkForExistence)
		{

		}
		return m_documentAccess.InsertDocumentToDatabase(_jsonData.extract(), _allowQueueing);
	}

	DataStorageResponse DataLakeAPI::searchInDataLakePartition(BsonViewOrValue _queryFilter, BsonViewOrValue _projectionQuery, int _limit)
	{

		
		return m_documentAccess.GetAllDocuments(_queryFilter, _projectionQuery, _limit);

	}

	DataStorageResponse DataLakeAPI::searchInDataLakePartition(BsonViewOrValue _queryFilter, mongocxx::options::find& _options)
	{
		return m_documentAccess.GetAllDocuments(_queryFilter, _options);
	}
	
	DataStorageResponse DataLakeAPI::searchInDataLakePartition(const std::string& _queryFilter, const std::string& _projectionQuery, int _limit)
	{
		return searchInDataLakePartition(bsoncxx::from_json(_queryFilter), bsoncxx::from_json(_projectionQuery), _limit);
	}

	int64_t DataLakeAPI::countInDataLakePartition(BsonViewOrValue _queryFilter)
	{
		auto& collection = getCollection();
		int64_t count =	collection.count_documents(_queryFilter);
		return count;
	}

	void DataLakeAPI::flushQueuedData()
	{
		m_docBase.FlushQueuedDocuments();
	}
}