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
		std::list<ot::BlockConnection> allConnections = blockEntity->getAllConnections();
		for (ot::Connector& connector : allConnectors)
		{
			if (connector.getConnectorType() == ot::ConnectorType::Source)
			{
				bool hasOutgoingConnection = CheckIfSourceHasOutgoingConnection(connector, allConnections);
				if (hasOutgoingConnection)
				{
					Pipeline newPipeline;
					if (_pipelineSources.find(blockEntity->getBlockID()) == _pipelineSources.end())
					{
						_pipelineSources[blockEntity->getBlockID()] = new PipelineSource(blockEntity);
					}
					newPipeline.SetSource(_pipelineSources[blockEntity->getBlockID()]);
					AddFiltersAndSinks(newPipeline, allConnections, allBlockEntities);
					pipelines.push_back(newPipeline);
				}
			}
		}
	}

}

void PipelineManager::AddFiltersAndSinks(Pipeline& newPipeline, std::list<ot::BlockConnection>& allBlockConnections, std::list<std::shared_ptr<EntityBlock>>& allBlockEntities)
{
	for (auto& blockConnection : allBlockConnections)
	{
		auto connection	= blockConnection.getConnection();
		for (auto& blockEntity : allBlockEntities)
		{
			if (connection.fromUID == blockEntity->getBlockID())
			{
				for (auto& connector : blockEntity->getAllConnectors())
				{
					if (connection.toConnectable == connector.getConnectorName())
					{
						if (connector.getConnectorType() == ot::ConnectorType::Filter)
						{
							newPipeline.AddFilter(PipelineFilter(blockEntity));
						}
						else if (connector.getConnectorType() == ot::ConnectorType::Sink)
						{
							if (_pipelineSinks.find(blockEntity->getBlockID()) == _pipelineSinks.end())
							{
								_pipelineSinks[blockEntity->getBlockID()] = new PipelineSink(blockEntity);
							}
							newPipeline.AddSink(_pipelineSinks[blockEntity->getBlockID()]);
							_pipelineSinks[blockEntity->getBlockID()]->setConnectorAssoziation(connection.fromConnectable, connection.toConnectable);
						}
						else
						{
							assert(0);
						}
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
			if (connection.getConnection().fromConnectable == connector.getConnectorName())
			{
				return true;
			}
		}
	}
	return false;
}
