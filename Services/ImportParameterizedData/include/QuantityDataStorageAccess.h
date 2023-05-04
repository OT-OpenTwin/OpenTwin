#pragma once
#include "ResultDataStorageAPI.h"
class QuantityDataStorageAccess
{
public:


	static DataStorageAPI::ResultDataStorageAPI GetStorageAccess() 
	{
		static DataStorageAPI::ResultDataStorageAPI resultCollectionAccess;
	};
	QuantityDataStorageAccess() = delete;
};
