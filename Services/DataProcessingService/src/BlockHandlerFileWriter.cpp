#include "BlockHandlerFileWriter.h"

#include "OTSystem/OTAssert.h"

#include "OTCore/FolderNames.h"
#include "OTCore/GenericDataStruct.h"
#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"

#include "EntityFileText.h"
#include "OTCommunication/ActionTypes.h"
#include "EntityBinaryData.h"
#include "StringConverter.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include "OTCore/EntityName.h"
#include "PipelineHandler.h"

BlockHandlerFileWriter::BlockHandlerFileWriter(EntityBlockFileWriter* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity, handlerMap)
{
	m_input = blockEntity->getConnectorInput();
	m_fileName = blockEntity->getFileName();
	if (m_fileName.empty())
	{
		const std::string entityName = blockEntity->getName();
		auto entityNameShort = ot::EntityName::getSubName(entityName);
		if (entityNameShort.has_value())
		{
			m_fileName = entityNameShort.value();
		}
		else
		{
			OT_LOG_W("Failed to deduce a fitting name for the text file");
		}
	}
	m_headline = blockEntity->getHeadline();
	m_fileType =blockEntity->getFileType();
}

bool BlockHandlerFileWriter::executeSpecialized()
{
	
	_uiComponent->displayMessage("Executing Filewriter Block: " + m_blockName + "\n");
	if (!m_headline.empty())
	{
		m_fileStream << m_headline<<"\n";
	}

	PipelineData* incommingPortData = m_dataPerPort[m_input.getConnectorName()];
	if (incommingPortData != nullptr)
	{
		m_fileStream <<	ot::json::toJson(incommingPortData->getData());
	}
	else
	{
		_uiComponent->displayMessage(getErrorDataPipelineNllptr() + "\n");
	}
		
	createFile();

	return true;
}

void BlockHandlerFileWriter::createFile()
{

	const std::string solverName =	PipelineHandler::getSolverName();
	const std::string resultFolder = solverName + "/" + ot::FolderNames::FilesFolder;
	const std::string fileName = CreateNewUniqueTopologyName(resultFolder, m_fileName);

	EntityFileText textFile(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService);
	textFile.setName(fileName);

	textFile.setFileProperties("", m_fileName, m_fileType);
	
	EntityBinaryData data(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService);
	const std::string fileContent = m_fileStream.str();

	data.setData(fileContent.c_str(), fileContent.size());
	data.StoreToDataBase();

	textFile.setData(data.getEntityID(),data.getEntityStorageVersion());
	textFile.StoreToDataBase();

	ot::ModelServiceAPI::addEntitiesToModel({ textFile.getEntityID() }, { textFile.getEntityStorageVersion() }, { false }, { data.getEntityID() }, { data.getEntityStorageVersion() }, { textFile.getEntityID() }, "Created text file.");
}

