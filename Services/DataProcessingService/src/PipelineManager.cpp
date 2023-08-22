#include "PipelineManager.h"
#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "Application.h"
#include "ClassFactory.h"
#include "Pipeline.h"

void PipelineManager::RunAll() 
{
	BusinessLogicHandler handler;
	auto blockEntities = GetAllBlockEntities();
	CreatePipelines(blockEntities);
	blockEntities.clear();
}

std::list<std::shared_ptr<EntityBlock>> PipelineManager::GetAllBlockEntities()
{
	std::string entityDict = "Processing Blocks";
	std::list<std::string> namesOfEntities = _modelComponent->getListOfFolderItems(entityDict);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(namesOfEntities, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactory classFactory;
	std::list<std::shared_ptr<EntityBlock>> returnValue;
	for (auto& entityInfo : entityInfos)
	{
		auto entityBase =_modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(),classFactory);
		auto blockEntity =	std::shared_ptr<EntityBlock>(dynamic_cast<EntityBlock*>(entityBase));
		assert(blockEntity != nullptr);
		returnValue.push_back(blockEntity);
	}

	return returnValue;
}

void PipelineManager::CreatePipelines(std::list<std::shared_ptr<EntityBlock>>& allBlockEntities)
{
	for (auto blockEntity : allBlockEntities)
	{
		std::list<ot::Connector> allConnectors = blockEntity->getAllConnectors();
		std::list<ot::BlockConnection> allConnections = blockEntity->getAllOutgoingConnections();

		for (ot::Connector& connector : allConnectors)
		{
			if (connector.getConnectorType() == ot::ConnectorType::Source)
			{
				bool hasOutgoingConnection = CheckIfSourceHasOutgoingConnection(connector, allConnections);
				if (hasOutgoingConnection)
				{
					//Pipeline newPipeline(blockEntity);
				}
			}
		}
	}

}

bool PipelineManager::CheckIfSourceHasOutgoingConnection(ot::Connector& connector, std::list<ot::BlockConnection>& allBlockConnections)
{
	return false;
}
