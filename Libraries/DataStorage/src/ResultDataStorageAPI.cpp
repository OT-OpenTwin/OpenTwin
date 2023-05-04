#include "stdafx.h"
#include "../include/ResultDataStorageAPI.h"

namespace DataStorageAPI
{
	ResultDataStorageAPI::ResultDataStorageAPI(const std::string& dataBaseURL, const std::string& collectionName)
		:documentAccess(dataBaseURL,collectionName + ".results")
	{}

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

}
