#include "stdafx.h"
#include "../include/ResultDataStorageAPI.h"
#include "Connection/ConnectionAPI.h"
#include "bsoncxx/json.hpp"
namespace DataStorageAPI
{
	ResultDataStorageAPI::ResultDataStorageAPI(const std::string& collectionName)
		:documentAccess("Projects", collectionName + ".results"), docBase("Projects", collectionName + ".results")
	{	

	}

	DataStorageResponse ResultDataStorageAPI::InsertDocumentToResultStorage(Document& jsonData, bool checkForExistence, bool allowQueueing)
	{
		auto jsonDataLength = jsonData.view().length();
		if (jsonDataLength > maxDocumentLength)
		{
			throw std::exception(("The result collection does not support documents with a size larger then " + std::to_string(maxDocumentLength)).c_str());
		}
		if (checkForExistence)
		{

		}
		return documentAccess.InsertDocumentToDatabase(jsonData.extract(), allowQueueing);
	}

	DataStorageResponse ResultDataStorageAPI::SearchInResultCollection(BsonViewOrValue queryFilter, BsonViewOrValue projectionQuery, int limit)
	{
		return documentAccess.GetAllDocuments(queryFilter, projectionQuery, limit);
	}

	DataStorageResponse ResultDataStorageAPI::SearchInResultCollection(const std::string& queryFilter, const std::string& projectionQuery, int limit)
	{
		return SearchInResultCollection(bsoncxx::from_json(queryFilter), bsoncxx::from_json(projectionQuery), limit);
	}

	void ResultDataStorageAPI::FlushQueuedData()
	{
		docBase.FlushQueuedDocuments();
	}
}