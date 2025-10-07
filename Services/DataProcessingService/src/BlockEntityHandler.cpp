#include "BlockEntityHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTCommunication/ActionTypes.h"
#include "ExternalDependencies.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include "Application.h"
#include "ClassFactoryBlock.h"
#include "ClassFactory.h"
#include "EntityBlockPython.h"
#include "AdvancedQueryBuilder.h"
#include "EntityBlockStorage.h"
#include "EntityBlockConnection.h"
#include "EntityBlockDisplay.h"
#include "EntityBlockFileWriter.h"
#include "SharedResources.h"
#include "OTCore/ComparisionSymbols.h"


void BlockEntityHandler::createBlockEntity(const std::string& editorName, const std::string& blockName,ot::Point2DD& position)
{
	ClassFactory& factory = Application::instance()->getClassFactory();
	EntityBase* baseEntity = factory.CreateEntity(blockName);
	assert(baseEntity != nullptr);		
	std::shared_ptr<EntityBlock> blockEntity (dynamic_cast<EntityBlock*>(baseEntity));

	std::string entName = CreateNewUniqueTopologyName(editorName + "/" + m_blockFolder, blockEntity->getBlockTitle());
	blockEntity->setName(entName);
	blockEntity->SetServiceInformation(Application::instance()->getBasicServiceInformation());
	blockEntity->setOwningService(OT_INFO_SERVICE_TYPE_DataProcessingService);
	blockEntity->setEntityID(_modelComponent->createEntityUID());
	blockEntity->setGraphicsScenePackageChildName(m_blockFolder);
	blockEntity->setEditable(true);

	std::unique_ptr<EntityCoordinates2D> blockCoordinates(new EntityCoordinates2D(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService));
	blockCoordinates->setCoordinates(position);
	blockCoordinates->StoreToDataBase();

	blockEntity->setCoordinateEntityID(blockCoordinates->getEntityID());
	
	initSpecialisedBlockEntity(blockEntity);

	blockEntity->StoreToDataBase();
	ot::ModelServiceAPI::addEntitiesToModel({ blockEntity->getEntityID() }, { blockEntity->getEntityStorageVersion() }, { false }, { blockCoordinates->getEntityID() }, { blockCoordinates->getEntityStorageVersion() }, { blockEntity->getEntityID() }, "Added Block: " + blockName);
}

void BlockEntityHandler::addBlockConnection(const std::list<ot::GraphicsConnectionCfg>& connections, const std::string& _baseFolderName)
{
	auto blockEntitiesByBlockID = findAllBlockEntitiesByBlockID(_baseFolderName);

	std::list< std::shared_ptr<EntityBlock>> entitiesForUpdate;
	ot::UIDList topoEntIDs, topoEntVers;
	const std::string connectionFolderName = _baseFolderName + "/" + m_connectionFolder;
	for (auto& connection : connections)
	{
		EntityBlockConnection connectionEntity(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService);
		connectionEntity.createProperties();
		ot::GraphicsConnectionCfg newConnection(connection);
		newConnection.setUid(connectionEntity.getEntityID());
		newConnection.setLineShape(ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine);

		const std::string connectionName = CreateNewUniqueTopologyName(connectionFolderName, "Connection", 1, false);
		connectionEntity.setName(connectionName);
		connectionEntity.setConnectionCfg(newConnection);
		connectionEntity.SetServiceInformation(Application::instance()->getBasicServiceInformation());
		connectionEntity.setOwningService(OT_INFO_SERVICE_TYPE_DataProcessingService);
		connectionEntity.setGraphicsScenePackageChildName(m_connectionFolder);
		connectionEntity.createProperties();

		connectionEntity.StoreToDataBase();
		topoEntIDs.push_back(connectionEntity.getEntityID());
		topoEntVers.push_back(connectionEntity.getEntityStorageVersion());
		
		bool originConnectorIsTypeOut(true), destConnectorIsTypeOut(true);

		if (blockEntitiesByBlockID.find(newConnection.getOriginUid()) != blockEntitiesByBlockID.end())
		{
			auto& blockEntity = blockEntitiesByBlockID[newConnection.getOriginUid()];
			
			originConnectorIsTypeOut = connectorHasTypeOut(blockEntity, newConnection.getOriginConnectable());
		}
		else
		{
			OT_LOG_EAS("Could not create connection since block " + std::to_string(newConnection.getOriginUid()) + " was not found");
			continue;
		}

		if (blockEntitiesByBlockID.find(newConnection.getDestinationUid()) != blockEntitiesByBlockID.end())
		{
			auto& blockEntity = blockEntitiesByBlockID[newConnection.getDestinationUid()];
			destConnectorIsTypeOut = connectorHasTypeOut(blockEntity, newConnection.getDestConnectable());
		}
		else
		{
			OT_LOG_EAS("Could not create connection since block " + std::to_string(newConnection.getDestinationUid()) + " was not found.");
			continue;
		}

		if (originConnectorIsTypeOut != destConnectorIsTypeOut)
		{
			blockEntitiesByBlockID[newConnection.getOriginUid()]->AddConnection(newConnection.getUid());
			entitiesForUpdate.push_back(blockEntitiesByBlockID[newConnection.getOriginUid()]);
			blockEntitiesByBlockID[newConnection.getDestinationUid()]->AddConnection(newConnection.getUid());
			entitiesForUpdate.push_back(blockEntitiesByBlockID[newConnection.getDestinationUid()]);
		}
		else
		{
			_uiComponent->displayMessage("Cannot create connection. One port needs to be an ingoing port while the other is an outgoing port.\n");
		}
	}

	if (entitiesForUpdate.size() != 0)
	{
		for (auto entityForUpdate : entitiesForUpdate)
		{
			entityForUpdate->StoreToDataBase();
			topoEntIDs.push_back(entityForUpdate->getEntityID());
			topoEntVers.push_back(entityForUpdate->getEntityStorageVersion());
		}
		ot::ModelServiceAPI::updateTopologyEntities(topoEntIDs, topoEntVers, "Added new connection to BlockEntities.");
	}
}

void BlockEntityHandler::orderUIToCreateBlockPicker()
{
	auto graphicsEditorPackage = BuildUpBlockPicker();
	ot::JsonDocument doc;
	ot::JsonObject pckgObj;
	graphicsEditorPackage->addToJsonObject(pckgObj, doc.GetAllocator());

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_FillItemPicker, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, doc.GetAllocator());

	Application::instance()->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	// Message is queued, no response here
	std::string tmp;
	_uiComponent->sendMessage(true, doc, tmp);
}


void BlockEntityHandler::initSpecialisedBlockEntity(std::shared_ptr<EntityBlock> blockEntity)
{
	EntityBlockPython* pythonBlock = dynamic_cast<EntityBlockPython*>(blockEntity.get());
	if (pythonBlock != nullptr)
	{
		ExternalDependencies dependency;
		pythonBlock->createProperties(ot::FolderNames::PythonScriptFolder, dependency.getPythonScriptFolderID());
		return;
	}
	
	EntityBlockDatabaseAccess* dbaBlock = dynamic_cast<EntityBlockDatabaseAccess*>(blockEntity.get());
	if (dbaBlock != nullptr)
	{
		dbaBlock->createProperties();
	}

	EntityBlockStorage* storage = dynamic_cast<EntityBlockStorage*>(blockEntity.get());
	if (storage)
	{
		storage->createProperties();
	}

	EntityBlockDisplay* display = dynamic_cast<EntityBlockDisplay*>(blockEntity.get());
	if(display)
	{
		display->createProperties();
	}

	EntityBlockFileWriter* fileWriter = dynamic_cast<EntityBlockFileWriter*>(blockEntity.get());
	if(fileWriter)
	{
		fileWriter->createProperties();
	}
}


ot::GraphicsPickerCollectionPackage* BlockEntityHandler::BuildUpBlockPicker()
{
	std::unique_ptr<ot::GraphicsPickerCollectionPackage> graphicsPicker(new ot::GraphicsPickerCollectionPackage());

	ot::GraphicsPickerCollectionCfg* controlBlockCollection = new ot::GraphicsPickerCollectionCfg("Control Blocks", "Control Blocks");
	ot::GraphicsPickerCollectionCfg* controlBlockDatabaseCollection = new ot::GraphicsPickerCollectionCfg("Database", "Database");
	ot::GraphicsPickerCollectionCfg* controlBlockVisualizationCollection = new ot::GraphicsPickerCollectionCfg("Visualization", "Visualization");

	//ot::GraphicsCollectionCfg* mathBlockCollection = new ot::GraphicsCollectionCfg("Mathematical Operations", "Mathematical Operations");
	ot::GraphicsPickerCollectionCfg* customizedBlockCollection = new ot::GraphicsPickerCollectionCfg("Customized Blocks", "Customized Blocks");
	

	controlBlockCollection->addChildCollection(controlBlockDatabaseCollection);
	controlBlockCollection->addChildCollection(controlBlockVisualizationCollection);

	EntityBlockPython pythonBlock(0, nullptr, nullptr, nullptr, nullptr, "");
	customizedBlockCollection->addItem(pythonBlock.getClassName(), pythonBlock.CreateBlockHeadline(), BlockEntities::SharedResources::getCornerImagePath() + EntityBlockPython::getIconName());

	EntityBlockDatabaseAccess dbAccessBlock(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockDatabaseCollection->addItem(dbAccessBlock.getClassName(), dbAccessBlock.CreateBlockHeadline(), BlockEntities::SharedResources::getCornerImagePath()+ EntityBlockDatabaseAccess::getIconName());

	EntityBlockDisplay displayBlock(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockVisualizationCollection->addItem(displayBlock.getClassName(), displayBlock.CreateBlockHeadline(), BlockEntities::SharedResources::getCornerImagePath() + EntityBlockDisplay::getIconName());

	EntityBlockStorage storage(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockDatabaseCollection->addItem(storage.getClassName(), storage.CreateBlockHeadline(), BlockEntities::SharedResources::getCornerImagePath() + EntityBlockStorage::getIconName());

	EntityBlockFileWriter fileWriter(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockDatabaseCollection->addItem(fileWriter.getClassName(), fileWriter.CreateBlockHeadline(), BlockEntities::SharedResources::getCornerImagePath() + EntityBlockFileWriter::getIconName());
	graphicsPicker->addCollection(controlBlockCollection);
	graphicsPicker->addCollection(customizedBlockCollection);
	//pckg->addCollection(mathBlockCollection);
	
	return graphicsPicker.release();
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
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), Application::instance()->getClassFactory());
		if (baseEntity != nullptr) //Otherwise not a BlockEntity, since ClassFactoryBlock does not handle others
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

bool BlockEntityHandler::connectorHasTypeOut(std::shared_ptr<EntityBlock> blockEntity, const std::string& connectorName)
{
	auto allConnectors = blockEntity->getAllConnectorsByName();
	const ot::ConnectorType connectorType = allConnectors[connectorName].getConnectorType();
	if (connectorType == ot::ConnectorType::UNKNOWN) { OT_LOG_EAS("Unset connectortype of connector: " + allConnectors[connectorName].getConnectorName()); }
	if (connectorType == ot::ConnectorType::In || connectorType == ot::ConnectorType::InOptional)
	{
		return false;
	}
	else
	{
		return true;
	}
}
