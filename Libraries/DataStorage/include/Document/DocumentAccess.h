// @otlicense

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

