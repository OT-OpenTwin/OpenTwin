// @otlicense
// File: DocumentAccess.h
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
#include <iostream>
#include <string>

#include "../Response/DataStorageResponse.h"
#include "DocumentAccessBase.h"
#include "Unittest/UnittestDocumentAccessBase.h"

#include <mongocxx/collection.hpp>
#include "bsoncxx/document/view_or_value.hpp"

using BsonViewOrValue = bsoncxx::document::view_or_value;
using string = std::string;

namespace DataStorageAPI
{
	class DocumentAccess
	{
	public: 
		__declspec(dllexport) DocumentAccess(string databaseName, string collectionName);
		__declspec(dllexport) DataStorageResponse InsertDocumentToDatabase(string jsonData, bool allowQueueing);
		__declspec(dllexport) DataStorageResponse InsertDocumentToDatabase(BsonViewOrValue bsonData, bool allowQueueing);

		__declspec(dllexport) DataStorageResponse InsertMultipleDocumentsToDatabase(std::vector<string> jsonData);

		__declspec(dllexport) DataStorageResponse GetDocument(string jsonQuery, string jsonProjectionQuery);
		__declspec(dllexport) DataStorageResponse GetDocument(BsonViewOrValue queryFilter, BsonViewOrValue projectionQuery);

		__declspec(dllexport) DataStorageResponse GetAllDocuments(string jsonQuery, string jsonProjectionQuery, int limit);
		__declspec(dllexport) DataStorageResponse GetAllDocuments(BsonViewOrValue queryFilter, BsonViewOrValue projectionQuery, int limit);
		__declspec(dllexport) DataStorageResponse GetAllDocuments(BsonViewOrValue _queryFilter, BsonViewOrValue _projectionQuery, BsonViewOrValue _sort,int limit);
		__declspec(dllexport) DataStorageResponse GetAllDocuments(BsonViewOrValue _queryFilter, mongocxx::options::find& _options);

		__declspec(dllexport) DataStorageResponse DeleteDocument(BsonViewOrValue queryFilter);

		__declspec(dllexport) ~DocumentAccess();

	protected:
		DocumentAccess(const std::string& collectionName) : docBase(new UnittestDocumentAccessBase(collectionName)) {};

	private:
		DocumentAccessBase* docBase = nullptr;
		string mongoDbName;
		string mongoCollectionName;
	};
}

