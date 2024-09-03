#pragma once
#include <string>
class CrossCollectionDatabaseWrapper
{
public:
	CrossCollectionDatabaseWrapper(const std::string& collectionName);
	~CrossCollectionDatabaseWrapper();
private:
	const std::string _oldCollectionName;
};

