#include "BlockEntityHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "OpenTwinCommunication/ActionTypes.h"


#include "Application.h"
#include "ClassFactoryBlock.h"

BlockEntityHandler& BlockEntityHandler::GetInstance()
{
	static BlockEntityHandler instance;
	return instance;
}

std::string BlockEntityHandler::CreateBlockEntity(const std::string& editorName, const std::string& blockName,ot::Point2DD& position)
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
	blockEntity->SetServiceInformation(Application::instance()->getBasicServiceInformation());
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

void BlockEntityHandler::AddBlockConnection(const ot::GraphicsConnectionPackage::ConnectionInfo& connection)
{
	std::list<std::string> blockEntities =_modelComponent->getListOfFolderItems(_blockFolder+"/Data Processing");
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(blockEntities, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactoryBlock classFactory;

	std::list<std::shared_ptr<EntityBlock>> changedEntities;
	for (ot::EntityInformation& entityInfo : entityInfos)
	{
		auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		std::shared_ptr<EntityBlock> blockEnt(dynamic_cast<EntityBlock*>(baseEnt));
		if (blockEnt->getBlockID() == connection.fromUID)
		{
			blockEnt->AddOutgoingConnection(connection);
			blockEnt->setModified();
			changedEntities.push_back(blockEnt);
		}
		else if (blockEnt->getBlockID() == connection.toUID)
		{
			blockEnt->AddIngoingConnection(connection);
			blockEnt->setModified();
			changedEntities.push_back(blockEnt);
		}
	}

	ot::UIDList topoEntID, topoEntVer;
	if(changedEntities.size() != 0)
	{
		for (auto blockEntity : changedEntities)
		{
			blockEntity->StoreToDataBase();
			topoEntID.push_back(blockEntity->getEntityID());
			topoEntVer.push_back(blockEntity->getEntityStorageVersion());
		}
			
		_modelComponent->updateTopologyEntities(topoEntID, topoEntVer);
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
