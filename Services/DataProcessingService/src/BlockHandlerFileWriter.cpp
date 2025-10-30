// @otlicense
// File: BlockHandlerFileWriter.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
#include "SolverReport.h"

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
		SolverReport::instance().addToContentAndDisplay(getErrorDataPipelineNllptr() + "\n", _uiComponent);
	}
		
	createFile();

	return true;
}

void BlockHandlerFileWriter::createFile()
{

	const std::string solverName =	SolverReport::instance().getSolverName();
	const std::string resultFolder = solverName + "/" + ot::FolderNames::FilesFolder;
	const std::string fileName = CreateNewUniqueTopologyName(resultFolder, m_fileName);

	EntityFileText textFile(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_MODEL);
	textFile.setName(fileName);

	textFile.setFileProperties("", m_fileName, m_fileType);
	
	EntityBinaryData data(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_MODEL);
	const std::string fileContent = m_fileStream.str();

	data.setData(fileContent.c_str(), fileContent.size());
	data.storeToDataBase();

	textFile.setDataEntity(data);
	textFile.storeToDataBase();

	ot::ModelServiceAPI::addEntitiesToModel({ textFile.getEntityID() }, { textFile.getEntityStorageVersion() }, { false }, { data.getEntityID() }, { data.getEntityStorageVersion() }, { textFile.getEntityID() }, "Created text file.");
}

std::string BlockHandlerFileWriter::getBlockType() const
{
	return "Filewriter Block";
}

