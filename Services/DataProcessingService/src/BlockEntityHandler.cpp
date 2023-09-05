#include "BlockEntityHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "BlockItemDatabaseAccess.h"
#include "BlockItemPlot1D.h"
#include "Application.h"
#include "ClassFactory.h"

std::shared_ptr<EntityBlock> BlockEntityHandler::CreateBlock(const std::string& editorName, const std::string& blockName, ot::UID itemID)
{
	std::shared_ptr<EntityBlock> blockEntity = nullptr;
	if (blockName == "Database access")
	{
		blockEntity = BlockItemDatabaseAccess::CreateBlockEntity();
	}
	else if(blockName == "Plot 1D")
	{
		blockEntity = BlockItemPlot1D::CreateBlockEntity();
	}
	else
	{
		return std::shared_ptr<EntityBlock>(nullptr);
	}

	assert(blockEntity != nullptr);
	
	std::string entName = CreateNewUniqueTopologyName(_blockFolder+"/"+ editorName, blockName);
	blockEntity->setName(entName);
	
	blockEntity->setEntityID(_modelComponent->createEntityUID());
	blockEntity->setOwningService(OT_INFO_SERVICE_TYPE_DataProcessingService);
	blockEntity->setBlockID(itemID);
	blockEntity->StoreToDataBase(); //Needs to be moved to the place in code where the location is being set

	return blockEntity;
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
