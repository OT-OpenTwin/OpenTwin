#include <stdafx.h>
#include "CrossCollectionDatabaseWrapper.h"
#include "DataBase.h"

CrossCollectionDatabaseWrapper::CrossCollectionDatabaseWrapper(const std::string& collectionName)
	:_oldCollectionName(DataBase::GetDataBase()->getProjectName())
{
	DataBase::GetDataBase()->setProjectName(collectionName);
	DataBase::GetDataBase()->RemovePrefetchedDocument(0);
}

CrossCollectionDatabaseWrapper::~CrossCollectionDatabaseWrapper()
{
	DataBase::GetDataBase()->setProjectName(_oldCollectionName);
	DataBase::GetDataBase()->RemovePrefetchedDocument(0);
}
