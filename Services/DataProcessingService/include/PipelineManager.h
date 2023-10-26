//#pragma once
//#include <list>
//
//#include "EntityBlock.h"
//#include "OpenTwinFoundation/BusinessLogicHandler.h"
//#include "Pipeline.h"
//
//class PipelineManager : public BusinessLogicHandler
//{
//public:
//	void RunAll();
//private:
//	std::list<Pipeline> pipelines;
//	std::map<std::string, PipelineSource*> _pipelineSources;
//	std::map<std::string, PipelineSink*> _pipelineSinks;
//
//
//	std::list<std::shared_ptr<EntityBlock>> GetAllBlockEntities();
//	void CreatePipelines(std::list<std::shared_ptr<EntityBlock>>& allBlockEntities);
//	void AddFiltersAndSinks(Pipeline& newPipeline, std::list<ot::BlockConnection>& allBlockConnections, std::list<std::shared_ptr<EntityBlock>>& allBlockEntities);
//	bool CheckIfSourceHasOutgoingConnection(ot::Connector& connector, std::list<ot::BlockConnection>& allBlockConnections);
//
//};
