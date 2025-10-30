// @otlicense
// File: ResultDataStorageAPI.h
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
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#include <string>

#include "Response/DataStorageResponse.h"
#include "bsoncxx/types/value.hpp"
#include "bsoncxx/builder/basic/document.hpp"
#include "bsoncxx/document/view_or_value.hpp"
#include "../include/Document/DocumentAccess.h"
#include "../include/Document/DocumentAccessBase.h"

#pragma warning (disable:4996)

using Document = bsoncxx::builder::basic::document;
using string = std::string;
using value = bsoncxx::types::value;
using BsonViewOrValue = bsoncxx::document::view_or_value;

namespace DataStorageAPI
{
	class __declspec(dllexport)  ResultDataStorageAPI
	{
	public:
		ResultDataStorageAPI(const std::string& _collectionName);
		ResultDataStorageAPI(const ResultDataStorageAPI& _other) = default;
		ResultDataStorageAPI(ResultDataStorageAPI&& _other) = default;
		ResultDataStorageAPI& operator=(const ResultDataStorageAPI& _other) = default;
		ResultDataStorageAPI& operator=(ResultDataStorageAPI&& _other) = default;
		~ResultDataStorageAPI() = default;

		DataStorageResponse insertDocumentToResultStorage(Document& _jsonData, bool _checkForExistence, bool _allowQueueing);
		DataStorageResponse searchInResultCollection(BsonViewOrValue _queryFilter, BsonViewOrValue _projectionQuery,int _limit);
		DataStorageResponse searchInResultCollection(BsonViewOrValue _queryFilter, mongocxx::options::find& _options);
		DataStorageResponse searchInResultCollection(const std::string& _queryFilter, const std::string& _projectionQuery, int _limit);
		void flushQueuedData();

		mongocxx::collection& getCollection() { return m_docBase.getCollection(); }

	private:
		int m_maxDocumentLength = 16776216;
		DocumentAccess m_documentAccess;
		DocumentAccessBase m_docBase;
	};
}