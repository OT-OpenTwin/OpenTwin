#include "BlockEntityHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCore/OwnerServiceGlobal.h"

#include "Application.h"
#include "ClassFactory.h"

BlockEntityHandler& BlockEntityHandler::GetInstance()
{
	static BlockEntityHandler instance;
	return instance;
}

ot::UID BlockEntityHandler::CreateBlockEntity(const std::string& editorName, const std::string& blockName,ot::Point2DD& position)
{
	std::shared_ptr<EntityBlock> blockEntity = nullptr;
	if (_blockEntityFactories.find(blockName) == _blockEntityFactories.end())
	{
		assert(0);
	}
	else
	{
		blockEntity= _blockEntityFactories[blockName]();
	}

	assert(blockEntity != nullptr);
	
	std::string entName = CreateNewUniqueTopologyName(_blockFolder+"/"+ editorName, blockName);
	blockEntity->setName(entName);
	blockEntity->SetOwnerServiceID(ot::OwnerServiceGlobal::instance().getId());
	blockEntity->setOwningService(OT_INFO_SERVICE_TYPE_DataProcessingService);
	blockEntity->setEntityID(_modelComponent->createEntityUID());
	blockEntity->setBlockID(_modelComponent->createEntityUID());
	blockEntity->SetGraphicsScenePackageName("Data Processing");
	std::unique_ptr<EntityCoordinates2D> blockCoordinates(new EntityCoordinates2D(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService));
	blockCoordinates->setCoordinates(position);
	blockCoordinates->StoreToDataBase();

	blockEntity->setCoordinateEntityID(blockCoordinates->getEntityID());
	blockEntity->StoreToDataBase();
	_modelComponent->addEntitiesToModel({ blockEntity->getEntityID() }, { blockEntity->getEntityStorageVersion() }, { false }, { blockCoordinates->getEntityID() }, { blockCoordinates->getEntityStorageVersion() }, { blockEntity->getEntityID() }, "Added Block: " + blockName);

	return blockEntity->getBlockID();
}

void BlockEntityHandler::AddBlockConnection(ot::UID idOrigin, ot::UID idDestination, const std::string& connectorOrigin, const std::string& connectorDest)
{
	std::list<std::string> blockEntities =_modelComponent->getListOfFolderItems(_blockFolder+"/Data Processing");
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(blockEntities, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactory classFactory;

	for (ot::EntityInformation& entityInfo : entityInfos)
	{
		auto baseEnt =	_modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		std::unique_ptr<EntityBlock> blockEnt(dynamic_cast<EntityBlock*>(baseEnt));
		if (blockEnt->getBlockID() == idOrigin)
		{
			blockEnt->AddOutgoingConnection(ot::BlockConnection(connectorOrigin, connectorDest,idDestination));
			blockEnt->setModified();
			blockEnt->StoreToDataBase();
			ot::UIDList topoEntID{ blockEnt->getEntityID() }, topoEntVer{ blockEnt->getEntityStorageVersion() }, dataEnt{};
			std::list<bool> forceVis{ false };
			_modelComponent->addEntitiesToModel(topoEntID, topoEntVer, forceVis, dataEnt, dataEnt, dataEnt, "Added connection to EntityBlock");
			break;
		}
	}
}

void BlockEntityHandler::RegisterBlockEntity(const std::string& key, std::function<std::shared_ptr<EntityBlock> ()> factoryMethod)
{
	if (_blockEntityFactories.find(key) == _blockEntityFactories.end())
	{
		_blockEntityFactories[key] = factoryMethod;
	}
	else
	{
		assert(0); //Double entry should not happen
	}
}
