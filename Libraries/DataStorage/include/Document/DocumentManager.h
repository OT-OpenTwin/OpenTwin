#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "../Response/DataStorageResponse.h"
#include "bsoncxx/types/value.hpp"
#include "bsoncxx/builder/basic/document.hpp"
#include "bsoncxx/document/view_or_value.hpp"

#pragma warning (disable:4996)

using Document = bsoncxx::builder::basic::document;
using string = std::string;
using value = bsoncxx::types::value;
using BsonViewOrValue = bsoncxx::document::view_or_value;

namespace DataStorageAPI
{
	class __declspec(dllexport) DocumentManager
	{
	public:
		
		DataStorageResponse InsertDocumentToDatabase(unsigned long long entityId, int version, string collectionName, 
			Document& jsonData, bool allowQueueing);

		DataStorageResponse InsertDocumentToFileStorage(unsigned long long entityId, int version, string collectionName, 
			Document& fileMetaData, string filePath, bool allowQueueing);

		DataStorageResponse InsertDocumentToFileStorage(unsigned long long entityId, int version, string collectionName, std::ifstream * source,
			Document& fileMetaData, bool allowQueueing);

		DataStorageResponse InsertDocumentToDatabaseOrFileStorage(unsigned long long entityId, unsigned long long version,
			string collectionName, Document& jsonData, bool checkForExistence, bool allowQueueing);

		DataStorageResponse GetAllDocument(string collectionName, std::map<string, value> filterPairs, std::vector<string> columnNames,
			bool includeDocumentId, int limit);
		DataStorageResponse GetDocument(string collectionName, std::map<string, value> filterPairs, std::vector<string> columnNames,
			bool includeDocumentId);

		string GetDocumentUsingFileStorage(BsonViewOrValue filterQuery, BsonViewOrValue projectionQuery);
		string GetDocumentUsingFTP(int siteId, string sourcePath);
		static const int MaxDocumentLength = 16776216;

	private:
		string GetDocumentLocalFilePath(int siteId, string relativePath);
		DataStorageResponse CheckDocumentEntityExists(unsigned long long entityId, unsigned long long version, string collectionName);
		string SaveDocumentToFileStorage(std::ifstream * source);
		string SaveDocumentToFileStorage(bsoncxx::document::view view, string fileExtension);
	};
}

