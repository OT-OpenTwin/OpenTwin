#pragma once
#include <string>
#include "ResultImportLogger.h"
#include "ResultDataStorageAPI.h"
class __declspec(dllexport) IndexHandler
{
public:
	IndexHandler(const std::string& _collectionName, ResultImportLogger& _logger);
	
	void createDefaultIndexes();

	ResultImportLogger& getLogger() { return m_logger; }

	const static std::vector<std::string>& getDefaultIndexes() 
	{ 
		static std::vector<std::string> m_defaultIndexes = { "Series","Quantity" };
		return m_defaultIndexes;
	}

	//! @brief Drop all indexes in the collection except for the default _id index.
	void dropAllIndexes();	

private:
	DataStorageAPI::ResultDataStorageAPI m_dataStorageAccess;

	std::vector<bool> m_defaultIndexesSet = { false,false };

	ResultImportLogger& m_logger;
	bool checkIfDefaultIndexesAreSet();
};
