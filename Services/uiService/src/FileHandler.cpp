#include "FileHandler.h"
#include <fstream>
#include <exception>
#include "EntityFileCSV.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCore/TextEncoding.h"

std::vector<char> FileHandler::ExtractFileContentAsBinary(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streampos size = file.tellg();

		std::shared_ptr<char> memBlock(new char[size]);
		file.seekg(0, std::ios::beg);
		file.read(memBlock.get(), size);
		file.close();
		std::vector<char> memBlockVector(memBlock.get(), memBlock.get() + size);
		return memBlockVector;
	}
	else
	{
		throw std::exception("File cannot be opened.");
	}
}

rapidjson::Document FileHandler::StoreFileInDataBase(const ot::UIDList& identifier)
{
	assert(identifier.size() == _filePaths.size() * 4);

	auto uid = identifier.begin();
	ot::UIDList topoID, topoVers, dataID, dataVers;

	for (const std::string absoluteFilePath : _filePaths)
	{
		std::string documentName = absoluteFilePath.substr(absoluteFilePath.find_last_of("/") + 1);
		std::string directoryPath = absoluteFilePath.substr(0, absoluteFilePath.find_last_of("/"));
		std::string documentType = documentName.substr(documentName.find(".") + 1);
		documentName = documentName.substr(0, documentName.find("."));

		auto newFile = CreateNewSourceEntity(documentType, *uid, _senderName);
		topoID.push_back(*uid);
		uid++;


		std::string entityName = CreateNewUniqueTopologyName(documentName);
		newFile->setName(entityName);
		newFile->setInitiallyHidden(false);
		newFile->setFileProperties(directoryPath, documentName, documentType);
		auto memBlock = ExtractFileContentAsBinary(absoluteFilePath);
		std::unique_ptr <EntityBinaryData> newData(new EntityBinaryData(*uid, newFile.get(), nullptr, nullptr, nullptr, _senderName));

		EntityFileText* newTextFile = dynamic_cast<EntityFileText*>(newFile.get());
		if (newTextFile!= nullptr)
		{
			ot::EncodingGuesser encoding;
			newTextFile->setTextEncoding(encoding(memBlock));
			newTextFile = nullptr;
		}
		dataID.push_back(*uid);
		uid++;
		
		newData->setData(memBlock.data(), memBlock.size());
		newData->StoreToDataBase(*uid);
		dataVers.push_back(*uid);
		uid++;

		newFile->setData(newData->getEntityID(), newData->getEntityStorageVersion());
		newFile->StoreToDataBase(*uid);
		topoVers.push_back(*uid);
		uid++;
	}

	return CreateReplyMessage(topoID,topoVers,dataID,dataVers);
}

std::string FileHandler::CreateNewUniqueTopologyName(const std::string& fileName)
{
	int count = 1;
	std::string fullFileName = _entityPath + "/" + fileName;
	while (std::find(_takenNames.begin(), _takenNames.end(), fullFileName) != _takenNames.end())
	{
		fullFileName = _entityPath + "/" + fileName + "_" + std::to_string(count);
		count++;
	}
	_takenNames.push_back(fullFileName);
	return fullFileName;
}

std::string FileHandler::ExtractFileNameFromPath(const std::string& absoluteFilePath)
{
	std::string documentName = absoluteFilePath.substr(absoluteFilePath.find_last_of("/") + 1);
	documentName = documentName.substr(0, documentName.find("."));
	return documentName;

	return std::string();
}

void FileHandler::SetNewFileImportRequest(const std::string&& senderURL, const std::string&& subsequentFunction, const std::string&& senderName, const std::list<std::string>&& takenNames, const std::list<std::string>&& filePaths, const std::string&& entityPath)
{
	_senderURL				= senderURL;
	_subsequentFunction		= subsequentFunction;
	_senderName				= senderName;
	_takenNames				= takenNames;
	_filePaths				= filePaths;
	_entityPath				= entityPath;
}

std::shared_ptr<EntityFile> FileHandler::CreateNewSourceEntity(const std::string& dataType, ot::UID entityID, const std::string& owner)
{
	EntityFile* newSource = nullptr;
	if (dataType == "txt" || dataType == "csv" || dataType == "CSV")
	{
		newSource = new EntityFileCSV(entityID, nullptr, nullptr, nullptr, nullptr, owner);
	}
	else
	{
		newSource = new EntityFile(entityID, nullptr, nullptr, nullptr, nullptr, owner);
	}
	return std::shared_ptr<EntityFile>(newSource);
}

rapidjson::Document FileHandler::CreateReplyMessage(const ot::UIDList& topoID, const ot::UIDList& topoVers, const ot::UIDList& dataID, const ot::UIDList& dataVers)
{
	rapidjson::Document reply;
	reply.SetObject();
	ot::rJSON::add(reply,OT_ACTION_MEMBER,OT_ACTION_CMD_MODEL_ExecuteFunction);
	ot::rJSON::add(reply, OT_ACTION_PARAM_MODEL_FunctionName, _subsequentFunction);
	ot::rJSON::add(reply, OT_ACTION_PARAM_FILE_OriginalName, _filePaths);
	ot::rJSON::add(reply, OT_ACTION_PARAM_MODEL_TopologyEntityIDList, topoID);
	ot::rJSON::add(reply, OT_ACTION_PARAM_MODEL_TopologyEntityVersionList, topoVers);
	ot::rJSON::add(reply, OT_ACTION_PARAM_MODEL_DataEntityIDList, dataID);
	ot::rJSON::add(reply, OT_ACTION_PARAM_MODEL_DataEntityVersionList, dataVers);
	return reply;
}
