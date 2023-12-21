#pragma once
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
	class ResultDataStorageAPI
	{
	public:

		__declspec(dllexport) ResultDataStorageAPI(const std::string& dataBaseURL, const std::string& collectionName);
		__declspec(dllexport) DataStorageResponse InsertDocumentToResultStorage(Document& jsonData, bool checkForExistence, bool allowQueueing);
		__declspec(dllexport) DataStorageResponse SearchInResultCollection(std::string& queryJsonData, std::string& projectionJsonData, int limit);
		__declspec(dllexport) void FlushQueuedData();

	private:
		const int maxDocumentLength = 16776216;
		DocumentAccess documentAccess;
		DocumentAccessBase docBase;
	};
}