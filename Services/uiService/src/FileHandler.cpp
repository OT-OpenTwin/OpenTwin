#include "FileHandler.h"
#include <fstream>
#include <exception>
#include "EntityFileCSV.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCore/TextEncoding.h"
#include "OTCore/EncodingGuesser.h"
#include "ClassFactory.h"

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

void FileHandler::addFileContentToJson(ot::JsonValue& _object, ot::JsonAllocator& _allocator, const std::string& _fileName)
{
	std::vector<char> fileContent =	ExtractFileContentAsBinary(_fileName);


}

ot::JsonDocument FileHandler::StoreFileInDataBase(const ot::UIDList& entityIDs, const ot::UIDList& entityVersions)
{
	assert(entityIDs.size() == _filePaths.size() * 2);
	assert(entityVersions.size() == _filePaths.size() * 2);

	auto uid = entityIDs.begin();
	auto version = entityVersions.begin();

	ot::UIDList topoID, topoVers, dataID, dataVers;

	for (const std::string absoluteFilePath : _filePaths)
	{
		std::string documentName = absoluteFilePath.substr(absoluteFilePath.find_last_of("/") + 1);
		std::string directoryPath = absoluteFilePath.substr(0, absoluteFilePath.find_last_of("/"));
		std::string documentType = documentName.substr(documentName.find(".") + 1);
		documentName = documentName.substr(0, documentName.find("."));

		auto newFile = CreateNewSourceEntity(*uid, _senderName);
		topoID.push_back(*uid);
		uid++;
		std::string entityName = CreateNewUniqueTopologyName(documentName);
		newFile->setName(entityName);
		newFile->setInitiallyHidden(false);
		newFile->setFileProperties(directoryPath, documentName, documentType);

		std::unique_ptr <EntityBinaryData> newData(new EntityBinaryData(*uid, newFile.get(), nullptr, nullptr, nullptr, _senderName));
		dataID.push_back(*uid);
		uid++;
		auto memBlock = ExtractFileContentAsBinary(absoluteFilePath);

		EntityFileText* newTextFile = dynamic_cast<EntityFileText*>(newFile.get());
		if (newTextFile!= nullptr)
		{
			ot::EncodingGuesser encoding;
			newTextFile->setTextEncoding(encoding(memBlock));
			newTextFile = nullptr;
		}
		
		newData->setData(memBlock.data(), memBlock.size());
		newData->StoreToDataBase(*version);
		dataVers.push_back(*version);
		version++;

		newFile->setData(newData->getEntityID(), newData->getEntityStorageVersion());
		newFile->StoreToDataBase(*version);
		topoVers.push_back(*version);
		version++;
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

void FileHandler::SetNewFileImportRequest(const std::string&& senderURL, const std::string&& subsequentFunction, const std::string&& senderName, const std::list<std::string>&& takenNames, const std::list<std::string>&& filePaths, const std::string&& entityPath, const std::string& entityType)
{
	_senderURL				= senderURL;
	_subsequentFunction		= subsequentFunction;
	_senderName				= senderName;
	_takenNames				= takenNames;
	_filePaths				= filePaths;
	_entityPath				= entityPath;
	_entityType				= entityType;
}

std::shared_ptr<EntityFile> FileHandler::CreateNewSourceEntity(ot::UID entityID, const std::string& owner)
{
	ClassFactory classFactory;
	EntityBase* newFileBase = classFactory.CreateEntity(_entityType);
	newFileBase->setEntityID(entityID);
	newFileBase->setOwningService(owner);
	EntityFile*	newFile = dynamic_cast<EntityFile*>(newFileBase);
	return std::shared_ptr<EntityFile>(newFile);
}

ot::JsonDocument FileHandler::CreateReplyMessage(const ot::UIDList& topoID, const ot::UIDList& topoVers, const ot::UIDList& dataID, const ot::UIDList& dataVers)
{
	ot::JsonDocument reply;
	reply.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteFunction, reply.GetAllocator()), reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(_subsequentFunction, reply.GetAllocator()), reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_FILE_OriginalName, ot::JsonArray(_filePaths, reply.GetAllocator()), reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityIDList, ot::JsonArray(topoID, reply.GetAllocator()), reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityVersionList, ot::JsonArray(topoVers, reply.GetAllocator()), reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_MODEL_DataEntityIDList, ot::JsonArray(dataID, reply.GetAllocator()), reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_MODEL_DataEntityVersionList, ot::JsonArray(dataVers, reply.GetAllocator()), reply.GetAllocator());
	return reply;
}
