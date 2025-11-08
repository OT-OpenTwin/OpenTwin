// @otlicense
// File: BlockEntityHandler.cpp
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

#include "BlockEntityHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTCommunication/ActionTypes.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include "Application.h"
#include "EntityBlockPython.h"
#include "AdvancedQueryBuilder.h"
#include "EntityBlockStorage.h"
#include "EntityBlockConnection.h"
#include "EntityBlockDisplay.h"
#include "EntityBlockFileWriter.h"
#include "SharedResources.h"
#include "OTCore/ComparisionSymbols.h"

void BlockEntityHandler::createBlockPicker()
{
	auto graphicsEditorPackage = BuildUpBlockPicker();
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_FillItemPicker, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, ot::JsonObject(graphicsEditorPackage, doc.GetAllocator()), doc.GetAllocator());

	// Message is queued, no response here
	std::string tmp;
	_uiComponent->sendMessage(true, doc, tmp);
}

ot::GraphicsPickerCollectionPackage BlockEntityHandler::BuildUpBlockPicker()
{
	ot::GraphicsPickerCollectionPackage graphicsPicker;
	graphicsPicker.setPickerKey(OT_INFO_SERVICE_TYPE_DataProcessingService);

	ot::GraphicsPickerCollectionCfg controlBlockCollection("Control Blocks", "Control Blocks");
	ot::GraphicsPickerCollectionCfg controlBlockDatabaseCollection("Database", "Database");
	ot::GraphicsPickerCollectionCfg controlBlockVisualizationCollection("Visualization", "Visualization");

	//ot::GraphicsPickerCollectionCfg* mathBlockCollection("Mathematical Operations", "Mathematical Operations");
	ot::GraphicsPickerCollectionCfg customizedBlockCollection("Customized Blocks", "Customized Blocks");
	
	EntityBlockPython pythonBlock;
	customizedBlockCollection.addItem(pythonBlock.getClassName(), pythonBlock.createBlockHeadline(), BlockEntities::SharedResources::getCornerImagePath() + EntityBlockPython::getIconName());

	EntityBlockDatabaseAccess dbAccessBlock;
	controlBlockDatabaseCollection.addItem(dbAccessBlock.getClassName(), dbAccessBlock.createBlockHeadline(), BlockEntities::SharedResources::getCornerImagePath()+ EntityBlockDatabaseAccess::getIconName());

	EntityBlockDisplay displayBlock;
	controlBlockVisualizationCollection.addItem(displayBlock.getClassName(), displayBlock.createBlockHeadline(), BlockEntities::SharedResources::getCornerImagePath() + EntityBlockDisplay::getIconName());

	EntityBlockStorage storage;
	controlBlockDatabaseCollection.addItem(storage.getClassName(), storage.createBlockHeadline(), BlockEntities::SharedResources::getCornerImagePath() + EntityBlockStorage::getIconName());

	EntityBlockFileWriter fileWriter;
	controlBlockDatabaseCollection.addItem(fileWriter.getClassName(), fileWriter.createBlockHeadline(), BlockEntities::SharedResources::getCornerImagePath() + EntityBlockFileWriter::getIconName());

	controlBlockCollection.addChildCollection(std::move(controlBlockDatabaseCollection));
	controlBlockCollection.addChildCollection(std::move(controlBlockVisualizationCollection));

	graphicsPicker.addCollection(std::move(controlBlockCollection));

	graphicsPicker.addCollection(std::move(customizedBlockCollection));
	//graphicsPicker.addCollection(std::move(mathBlockCollection));
	
	return graphicsPicker;
}

std::map<ot::UID, std::shared_ptr<EntityBlock>> BlockEntityHandler::findAllBlockEntitiesByBlockID(const std::string& _folderName)
{
	const std::string fullBlockFolderFile = _folderName + "/" + m_blockFolder;
	std::list<std::string> blockItemNames = ot::ModelServiceAPI::getListOfFolderItems(fullBlockFolderFile);
	
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(blockItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	
	std::map<ot::UID, std::shared_ptr<EntityBlock>> blockEntitiesByBlockID;
	for (auto& entityInfo : entityInfos)
	{
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());
		if (baseEntity != nullptr)
		{
			std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));
			if (blockEntity != nullptr)
			{
				blockEntitiesByBlockID[blockEntity->getEntityID()] = blockEntity;
			}
		}
	}
	return blockEntitiesByBlockID;
}

std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> BlockEntityHandler::findAllEntityBlockConnections(const std::string& _folderName) {
	const std::string fullFolderName = _folderName + "/" + m_connectionsFolder;
	std::list<std::string> connectionItemNames = ot::ModelServiceAPI::getListOfFolderItems(fullFolderName);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(connectionItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);

	std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> entityBlockConnectionsByBlockID;
	EntityBlockConnection temp;
	for (auto& entityInfo : entityInfos) {
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());
		if (baseEntity != nullptr && baseEntity->getClassName() == temp.getClassName()) {
			std::shared_ptr<EntityBlockConnection> blockEntityConnection(dynamic_cast<EntityBlockConnection*>(baseEntity));
			if (blockEntityConnection != nullptr)
			{
				entityBlockConnectionsByBlockID[blockEntityConnection->getEntityID()] = blockEntityConnection;
			}
		}
	}

	return entityBlockConnectionsByBlockID;
}
