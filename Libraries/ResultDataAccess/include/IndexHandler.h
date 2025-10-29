// @otlicense

#pragma once
#include <string>
#include "ResultImportLogger.h"
#include "ResultDataStorageAPI.h"
#include "OTServiceFoundation/UiComponent.h"
class __declspec(dllexport) IndexHandler
{
public:
	IndexHandler(const std::string& _collectionName);
	
	void createDefaultIndexes();

	const static std::vector<std::string>& getDefaultIndexes() 
	{ 
		static std::vector<std::string> m_defaultIndexes = { "Quantity" ,"Series"};
		return m_defaultIndexes;
	}

	//! @brief Drop all indexes in the collection except for the default _id index.
	void dropAllIndexes();	

private:
	DataStorageAPI::ResultDataStorageAPI m_dataStorageAccess;
	std::vector<bool> m_defaultIndexesSet = { false,false };

	bool checkIfDefaultIndexesAreSet();
};
