#include <stdafx.h>
#include "Unittest/UnittestDocumentAccessBase.h"
#include "Connection/ConnectionAPI.h"

UnittestDocumentAccessBase::UnittestDocumentAccessBase(const std::string& collectionName)
{
	DataStorageAPI::ConnectionAPI::getInstance().setMongoInstance( &std::cout);

	// Now test, whetehr the connection is working
	mongoCollection =	DataStorageAPI::ConnectionAPI::getInstance().getCollection(_dbName, collectionName);
}

