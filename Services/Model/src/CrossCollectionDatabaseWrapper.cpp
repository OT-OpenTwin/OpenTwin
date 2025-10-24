#include <stdafx.h>
#include "CrossCollectionDatabaseWrapper.h"
#include "DataBase.h"

CrossCollectionDatabaseWrapper::CrossCollectionDatabaseWrapper(const std::string& collectionName)
	:_oldCollectionName(DataBase::instance().getCollectionName())
{
	DataBase::instance().setCollectionName(collectionName);
	DataBase::instance().removePrefetchedDocument(0);
}

CrossCollectionDatabaseWrapper::~CrossCollectionDatabaseWrapper()
{
	DataBase::instance().setCollectionName(_oldCollectionName);
	DataBase::instance().removePrefetchedDocument(0);
}
