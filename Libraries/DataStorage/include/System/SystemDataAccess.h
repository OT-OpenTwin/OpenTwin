// @otlicense

#pragma once
#include <iostream>
#include <string>

#include "../Response/DataStorageResponse.h"

using string = std::string;

namespace DataStorageAPI
{
	class SystemDataAccess
	{
	public:
		__declspec(dllexport) SystemDataAccess(string databaseName, string collectionName);
		__declspec(dllexport) DataStorageResponse GetSystemDetails(int siteId);

	private:
		std::string _mongoDbName;
		std::string _mongoCollectionName;
	};
}

