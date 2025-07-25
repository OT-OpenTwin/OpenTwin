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

BlockHandlerFileWriter::BlockHandlerFileWriter(EntityBlockFileWriter* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity, handlerMap)
{
	m_input = blockEntity->getConnectorInput();
	m_fileName = blockEntity->getFileName();
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
	const std::string fileName = CreateNewUniqueTopologyName(ot::FolderNames::FilesFolder, m_fileName);

	EntityFileText textFile(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService);
	textFile.setName(fileName);

	textFile.setFileProperties("", m_fileName, m_fileType);
	
	EntityBinaryData data(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService);
	const std::string fileContent = m_fileStream.str();

	data.setData(fileContent.c_str(), fileContent.size());
	data.StoreToDataBase();

	textFile.setData(data.getEntityID(),data.getEntityStorageVersion());
	textFile.StoreToDataBase();

	ot::ModelServiceAPI::addEntitiesToModel({ textFile.getEntityID() }, { textFile.getEntityStorageVersion() }, { false }, { data.getEntityID() }, { data.getEntityStorageVersion() }, { textFile.getEntityID() }, "Created text file.");
}

