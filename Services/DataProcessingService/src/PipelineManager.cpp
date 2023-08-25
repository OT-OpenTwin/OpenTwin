#include "PipelineManager.h"
#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "Application.h"
#include "ClassFactory.h"


void PipelineManager::RunAll() 
{
	auto blockEntities = GetAllBlockEntities();
	CreatePipelines(blockEntities);
	blockEntities.clear();

	for (Pipeline& pipeline : pipelines)
	{
		pipeline.RunPipeline();
	}
}

std::list<std::shared_ptr<EntityBlock>> PipelineManager::GetAllBlockEntities()
{
	std::string entityDict = "Blocks/Data Processing";
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
					Pipeline newPipeline;
					newPipeline.SetSource(new PipelineSource(blockEntity));
					AddFiltersAndSinks(newPipeline, allConnections, allBlockEntities);
					pipelines.push_back(newPipeline);
				}
			}
		}
	}

}

void PipelineManager::AddFiltersAndSinks(Pipeline& newPipeline, std::list<ot::BlockConnection>& allBlockConnections, std::list<std::shared_ptr<EntityBlock>>& allBlockEntities)
{
	for (auto& connection : allBlockConnections)
	{
		for (auto& blockEntity : allBlockEntities)
		{
			if (connection.getIDDestination() == blockEntity->getBlockID())
			{
				for (auto& connector : blockEntity->getAllConnectors())
				{
					if (connector.getConnectorType() == ot::ConnectorType::Filter)
					{
						newPipeline.AddFilter(PipelineFilter(blockEntity));
					}
					else if (connector.getConnectorType() == ot::ConnectorType::Sink)
					{
						newPipeline.AddSink(PipelineSink(blockEntity));
					}
					else
					{
						assert(0);
					}
				}
				break;
			}
		}
	}
}

bool PipelineManager::CheckIfSourceHasOutgoingConnection(ot::Connector& connector, std::list<ot::BlockConnection>& allBlockConnections)
{
	if (allBlockConnections.size() == 0)
	{
		return false;
	}
	else
	{
		for (auto& connection : allBlockConnections)
		{
			if (connection.getConnectorOrigin() == connector.getConnectorName())
			{
				return true;
			}
		}
	}
	return false;
}
