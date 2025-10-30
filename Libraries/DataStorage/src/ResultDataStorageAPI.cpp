// @otlicense
// File: ResultDataStorageAPI.cpp
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

#include "stdafx.h"
#include "../include/ResultDataStorageAPI.h"
#include "Connection/ConnectionAPI.h"
#include "bsoncxx/json.hpp"
namespace DataStorageAPI
{
	ResultDataStorageAPI::ResultDataStorageAPI(const std::string& _collectionName)
		:m_documentAccess("Projects", _collectionName + ".results"), m_docBase("Projects", _collectionName + ".results")
	{	

	}

	DataStorageResponse ResultDataStorageAPI::insertDocumentToResultStorage(Document& _jsonData, bool _checkForExistence, bool _allowQueueing)
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

	DataStorageResponse ResultDataStorageAPI::searchInResultCollection(BsonViewOrValue _queryFilter, BsonViewOrValue _projectionQuery, int _limit)
	{

		
		return m_documentAccess.GetAllDocuments(_queryFilter, _projectionQuery, _limit);

	}

	DataStorageResponse ResultDataStorageAPI::searchInResultCollection(BsonViewOrValue _queryFilter, mongocxx::options::find& _options)
	{
		return m_documentAccess.GetAllDocuments(_queryFilter, _options);
	}
	
	DataStorageResponse ResultDataStorageAPI::searchInResultCollection(const std::string& _queryFilter, const std::string& _projectionQuery, int _limit)
	{
		return searchInResultCollection(bsoncxx::from_json(_queryFilter), bsoncxx::from_json(_projectionQuery), _limit);
	}

	void ResultDataStorageAPI::flushQueuedData()
	{
		m_docBase.FlushQueuedDocuments();
	}
}