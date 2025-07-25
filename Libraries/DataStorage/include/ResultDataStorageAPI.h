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
		ResultDataStorageAPI(const std::string& collectionName);
		ResultDataStorageAPI(const ResultDataStorageAPI& other) = default;
		ResultDataStorageAPI(ResultDataStorageAPI&& other) = default;
		ResultDataStorageAPI& operator=(const ResultDataStorageAPI& other) = default;
		ResultDataStorageAPI& operator=(ResultDataStorageAPI&& other) = default;
		~ResultDataStorageAPI() = default;

		DataStorageResponse InsertDocumentToResultStorage(Document& jsonData, bool checkForExistence, bool allowQueueing);
		DataStorageResponse SearchInResultCollection(BsonViewOrValue _queryFilter, BsonViewOrValue _projectionQuery,int _limit);
		DataStorageResponse SearchInResultCollection(BsonViewOrValue _queryFilter, BsonViewOrValue _projectionQuery, BsonViewOrValue _sort, int _limit);
		DataStorageResponse SearchInResultCollection(const std::string& queryFilter, const std::string& projectionQuery, int limit);
		void FlushQueuedData();

	private:
		int maxDocumentLength = 16776216;
		DocumentAccess documentAccess;
		DocumentAccessBase docBase;
	};
}