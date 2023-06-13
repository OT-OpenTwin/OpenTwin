#pragma once
#include <vector>
#include <string>
#include <list>
#include <memory>
#include "OpenTwinCore/CoreTypes.h"
#include "EntityParameterizedDataSource.h"

class FileHandler
{
public:
	std::string StoreFileInDataBase(const std::string& absoluteFilePath, const std::string entityName, const ot::UIDList& uids, const std::string& owner);
	std::string CreateNewUniqueTopologyName(std::list<std::string>& takenNames, const std::string& fileName, const std::string& fileDestinationPath);
	std::string ExtractFileNameFromPath(const std::string& absoluteFilePath);

private:
	std::vector<char> ExtractFileContentAsBinary(const std::string& fileName);
	std::shared_ptr<EntityParameterizedDataSource> CreateNewSourceEntity(const std::string& dataType, ot::UID entityID, const std::string& owner);
};
