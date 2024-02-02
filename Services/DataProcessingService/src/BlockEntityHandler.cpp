#include "BlockEntityHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "OTCommunication/ActionTypes.h"
#include "ExternalDependencies.h"

#include "Application.h"
#include "ClassFactoryBlock.h"
#include "ClassFactory.h"
#include "EntityBlockDatabaseAccess.h"
#include "EntityBlockPlot1D.h"
#include "EntityBlockPython.h"
#include "AdvancedQueryBuilder.h"
#include "EntityBlockDataDimensionReducer.h"
#include "EntityBlockStorage.h"

void BlockEntityHandler::CreateBlockEntity(const std::string& editorName, const std::string& blockName,ot::Point2DD& position)
{
	ClassFactoryBlock factory;
	EntityBase* baseEntity = factory.CreateEntity(blockName);
	assert(baseEntity != nullptr);		
	std::shared_ptr<EntityBlock> blockEntity (dynamic_cast<EntityBlock*>(baseEntity));

	std::string entName = CreateNewUniqueTopologyName(_blockFolder+"/"+ editorName, blockEntity->getBlockTitle());
	blockEntity->setName(entName);
	blockEntity->SetServiceInformation(Application::instance()->getBasicServiceInformation());
	blockEntity->setOwningService(OT_INFO_SERVICE_TYPE_DataProcessingService);
	blockEntity->setEntityID(_modelComponent->createEntityUID());
	blockEntity->SetGraphicsScenePackageName(_packageName);
	
	std::unique_ptr<EntityCoordinates2D> blockCoordinates(new EntityCoordinates2D(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService));
	blockCoordinates->setCoordinates(position);
	blockCoordinates->StoreToDataBase();

	blockEntity->setCoordinateEntityID(blockCoordinates->getEntityID());
	
	InitSpecialisedBlockEntity(blockEntity);

	blockEntity->StoreToDataBase();
	_modelComponent->addEntitiesToModel({ blockEntity->getEntityID() }, { blockEntity->getEntityStorageVersion() }, { false }, { blockCoordinates->getEntityID() }, { blockCoordinates->getEntityStorageVersion() }, { blockEntity->getEntityID() }, "Added Block: " + blockName);
}

void BlockEntityHandler::AddBlockConnection(const std::list<ot::GraphicsConnectionCfg>& connections)
{
	auto blockEntitiesByBlockID = findAllBlockEntitiesByBlockID();

	std::list< std::shared_ptr<EntityBlock>> entitiesForUpdate;
	for (auto& connection : connections)
	{
		bool originConnectorIsTypeOut(true), destConnectorIsTypeOut(true);

		if (blockEntitiesByBlockID.find(connection.originUid()) != blockEntitiesByBlockID.end())
		{
			auto& blockEntity = blockEntitiesByBlockID[connection.originUid()];
			
			originConnectorIsTypeOut = connectorHasTypeOut(blockEntity, connection.originConnectable());
		}
		else
		{
			OT_LOG_EAS("Could not create connection since block " + connection.originUid() + " was not found");
			continue;
		}

		if (blockEntitiesByBlockID.find(connection.destUid()) != blockEntitiesByBlockID.end())
		{
			auto& blockEntity = blockEntitiesByBlockID[connection.destUid()];
			destConnectorIsTypeOut = connectorHasTypeOut(blockEntity, connection.destConnectable());
		}
		else
		{
			OT_LOG_EAS("Could not create connection since block " + connection.destUid() + " was not found.");
			continue;
		}

		if (originConnectorIsTypeOut != destConnectorIsTypeOut)
		{
			blockEntitiesByBlockID[connection.originUid()]->AddConnection(connection);
			entitiesForUpdate.push_back(blockEntitiesByBlockID[connection.originUid()]);
			blockEntitiesByBlockID[connection.destUid()]->AddConnection(connection);
			entitiesForUpdate.push_back(blockEntitiesByBlockID[connection.destUid()]);
		}
		else
		{
			_uiComponent->displayMessage("Cannot create connection. One port needs to be an ingoing port while the other is an outgoing port.\n");
		}
	}

	if (entitiesForUpdate.size() != 0)
	{
		ot::UIDList topoEntIDs, topoEntVers;

		for (auto entityForUpdate : entitiesForUpdate)
		{
			entityForUpdate->StoreToDataBase();
			topoEntIDs.push_back(entityForUpdate->getEntityID());
			topoEntVers.push_back(entityForUpdate->getEntityStorageVersion());
		}
		_modelComponent->updateTopologyEntities(topoEntIDs, topoEntVers, "Added new connection to BlockEntities.");
	}
}

void BlockEntityHandler::OrderUIToCreateBlockPicker()
{
	auto graphicsEditorPackage = BuildUpBlockPicker();
	ot::JsonDocument doc;
	ot::JsonObject pckgObj;
	graphicsEditorPackage->addToJsonObject(pckgObj, doc.GetAllocator());

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, doc.GetAllocator());

	Application::instance()->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	// Message is queued, no response here
	_uiComponent->sendMessage(true, doc);
}

void BlockEntityHandler::UpdateBlockPosition(const std::string& blockID, ot::Point2DD& position, ClassFactory* classFactory)
{
	auto blockEntitiesByBlockID = findAllBlockEntitiesByBlockID();
	if (blockEntitiesByBlockID.find(blockID) == blockEntitiesByBlockID.end())
	{
		OT_LOG_EAS("Position of block item cannot be updated because a block with id: " + blockID + " was not found");
	}
	auto blockEntity = blockEntitiesByBlockID[blockID];
	std::list<ot::EntityInformation> entityInfos;
	ot::UIDList entityList{ blockEntity->getCoordinateEntityID() };
	_modelComponent->getEntityInformation(entityList, entityInfos);
	auto entBase = _modelComponent->readEntityFromEntityIDandVersion(entityInfos.begin()->getID(), entityInfos.begin()->getVersion(), *classFactory);
	std::unique_ptr<EntityCoordinates2D> coordinateEnt(dynamic_cast<EntityCoordinates2D*>(entBase));
	coordinateEnt->setCoordinates(position);
	coordinateEnt->StoreToDataBase();
	_modelComponent->addEntitiesToModel({}, {}, {}, { coordinateEnt->getEntityID() }, { coordinateEnt->getEntityStorageVersion() }, { blockEntity->getEntityID() }, "Update BlockItem position");
}

void BlockEntityHandler::InitSpecialisedBlockEntity(std::shared_ptr<EntityBlock> blockEntity)
{
	EntityBlockPython* pythonBlock = dynamic_cast<EntityBlockPython*>(blockEntity.get());
	if (pythonBlock != nullptr)
	{
		ExternalDependencies dependency;
		pythonBlock->createProperties(ot::FolderNames::PythonScriptFolder, dependency.getPythonScriptFolderID());
		return;
	}
	EntityBlockPlot1D* plotBlock = dynamic_cast<EntityBlockPlot1D*>(blockEntity.get());
	if (plotBlock != nullptr)
	{
		plotBlock->createProperties();
	}

	EntityBlockDatabaseAccess* dbaBlock = dynamic_cast<EntityBlockDatabaseAccess*>(blockEntity.get());
	if (dbaBlock != nullptr)
	{
		auto comparators = AdvancedQueryBuilder::getComparators();
		comparators.push_back(getQueryForRangeSelection());
		comparators.push_back(" ");
		dbaBlock->createProperties(comparators);
	}

	EntityBlockDataDimensionReducer* dataAR = dynamic_cast<EntityBlockDataDimensionReducer*>(blockEntity.get());
	if (dataAR)
	{
		dataAR->createProperties();
	}

	EntityBlockStorage* storage = dynamic_cast<EntityBlockStorage*>(blockEntity.get());
	if (storage)
	{
		storage->createProperties();
	}
}

ot::GraphicsNewEditorPackage* BlockEntityHandler::BuildUpBlockPicker()
{
	ot::GraphicsNewEditorPackage* pckg = new ot::GraphicsNewEditorPackage(_packageName, _packageName);
	ot::GraphicsCollectionCfg* controlBlockCollection = new ot::GraphicsCollectionCfg("Control Blocks", "Control Blocks");
	ot::GraphicsCollectionCfg* controlBlockDatabaseCollection = new ot::GraphicsCollectionCfg("Database", "Database");
	ot::GraphicsCollectionCfg* controlBlockVisualizationCollection = new ot::GraphicsCollectionCfg("Visualization", "Visualization");

	//ot::GraphicsCollectionCfg* mathBlockCollection = new ot::GraphicsCollectionCfg("Mathematical Operations", "Mathematical Operations");
	ot::GraphicsCollectionCfg* customizedBlockCollection = new ot::GraphicsCollectionCfg("Customized Blocks", "Customized Blocks");

	controlBlockCollection->addChildCollection(controlBlockDatabaseCollection);
	controlBlockCollection->addChildCollection(controlBlockVisualizationCollection);

	EntityBlockPython pythonBlock(0, nullptr, nullptr, nullptr, nullptr, "");
	customizedBlockCollection->addItem(pythonBlock.CreateBlockCfg());

	EntityBlockDatabaseAccess dbAccessBlock(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockDatabaseCollection->addItem(dbAccessBlock.CreateBlockCfg());

	EntityBlockPlot1D plotBlock(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockVisualizationCollection->addItem(plotBlock.CreateBlockCfg());

	EntityBlockDataDimensionReducer dimensionReducer(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockDatabaseCollection->addItem(dimensionReducer.CreateBlockCfg());

	EntityBlockStorage storage(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockDatabaseCollection->addItem(storage.CreateBlockCfg());

	pckg->addCollection(controlBlockCollection);
	pckg->addCollection(customizedBlockCollection);
	//pckg->addCollection(mathBlockCollection);

	return pckg;
}

std::map<std::string, std::shared_ptr<EntityBlock>> BlockEntityHandler::findAllBlockEntitiesByBlockID()
{
	std::list<std::string> blockItemNames = _modelComponent->getListOfFolderItems(_blockFolder + "/" + _packageName, true);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(blockItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactoryBlock classFactory;

	std::map<std::string, std::shared_ptr<EntityBlock>> blockEntitiesByBlockID;
	for (auto& entityInfo : entityInfos)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		if (baseEntity != nullptr) //Otherwise not a BlockEntity, since ClassFactoryBlock does not handle others
		{
			std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));
			assert(blockEntity != nullptr);
			blockEntitiesByBlockID[blockEntity->getBlockID()] = blockEntity;
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
