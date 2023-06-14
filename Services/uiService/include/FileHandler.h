/*****************************************************************//**
 * \file   FileHandler.h
 * \brief  Helperfunctions for file related operations.
 * 
 * \author Wagner
 * \date   June 2023
 *********************************************************************/
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
	/// <summary>
	/// Stores a file as binary in the database.
	/// </summary>
	/// <param name="absoluteFilePath"></param>
	/// <param name="entityName"></param>
	/// <param name="uids"> Requires two UIDs per file. One for the data entity and one for the topology entity.</param>
	/// <param name="owner"> Owner of the new entities. Typically the requesting service. </param>
	/// <returns> Name of the stored file. </returns>
	std::string StoreFileInDataBase(const std::string& absoluteFilePath, const std::string entityName, const ot::UIDList& uids, const std::string& owner);
	/// <summary>
	/// Finds the next possible name for the topology entity. Already taken names are investigated and the new name will be the next, not taken <fileDestinationPath>/<filename>_<x>.
	/// </summary>
	/// <param name="takenNames"> Names of all entities that are at the same location in the project navigation. </param>
	/// <param name="fileName"></param>
	/// <param name="fileDestinationPath"></param>
	/// <returns> Unique name for topology entity. </returns>
	std::string CreateNewUniqueTopologyName(std::list<std::string>& takenNames, const std::string& fileName, const std::string& fileDestinationPath);
	/// <summary>
	/// Extracts the file name from the absolute path and trims the file type as well.
	/// </summary>
	/// <param name="absoluteFilePath"></param>
	/// <returns> file name. </returns>
	std::string ExtractFileNameFromPath(const std::string& absoluteFilePath);

private:
	std::vector<char> ExtractFileContentAsBinary(const std::string& fileName);
	std::shared_ptr<EntityParameterizedDataSource> CreateNewSourceEntity(const std::string& dataType, ot::UID entityID, const std::string& owner);
};
