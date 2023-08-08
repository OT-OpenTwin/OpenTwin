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
#include "EntityFile.h"
#include "OpenTwinCore/rJSON.h"

class FileHandler
{
public:
	FileHandler() {};
	FileHandler(const FileHandler& other) = delete;
	FileHandler(const FileHandler&& other) = delete;
	FileHandler& operator=(const FileHandler& other) = delete;
	FileHandler& operator=(const FileHandler&& other) = delete;

	rapidjson::Document StoreFileInDataBase(const ot::UIDList& identifier);
	
	/// <summary>
	/// Extracts the file name from the absolute path and trims the file type as well.
	/// </summary>
	/// <param name="absoluteFilePath"></param>
	/// <returns> file name. </returns>
	std::string ExtractFileNameFromPath(const std::string& absoluteFilePath);

	void SetNewFileImportRequest(const std::string&& senderURL, const std::string&& subsequentFunction, const std::string&& senderName, const std::list<std::string>&& takenNames, const std::list<std::string>&& filePaths, const std::string&& entityPath);
	const std::string& GetStoreFileFunctionName() { return functionName; }
	const std::string& GetSenderURL() { return _senderURL; }

private:
	const std::string functionName = "StoreTableEntities";
	std::string _senderURL = "";
	std::string _subsequentFunction = "";
	std::string _senderName = "";
	std::list<std::string> _takenNames;
	std::list<std::string> _filePaths;
	std::string _entityPath = "";

	std::vector<char> ExtractFileContentAsBinary(const std::string& fileName);

	/// /// <summary>
	/// Finds the next possible name for the topology entity. Already taken names are investigated and the new name will be the next, not taken <fileDestinationPath>/<filename>_<x>.
	/// </summary>
	/// <param name="takenNames"> Names of all entities that are at the same location in the project navigation. </param>
	/// <param name="fileName"></param>
	/// <param name="fileDestinationPath"></param>
	/// <returns> Unique name for topology entity. </returns>
	std::string CreateNewUniqueTopologyName(const std::string& fileName);
	std::shared_ptr<EntityFile> CreateNewSourceEntity(const std::string& dataType, ot::UID entityID, const std::string& owner);

	rapidjson::Document	CreateReplyMessage(const ot::UIDList& topoID, const ot::UIDList& topoVers, const ot::UIDList& dataID, const ot::UIDList& dataVers);
};
