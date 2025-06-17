#pragma once
#include <list>
#include "OTCore/Variable.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "GraphNode.h"
#include "EntityBlock.h"
#include "PipelineData.h"

class BlockHandler : public BusinessLogicHandler
{
public:
	using HandlerMap = std::map<std::shared_ptr<GraphNode>, std::shared_ptr<BlockHandler>>;

	BlockHandler(EntityBlock* blockEntity, const HandlerMap& allHandler);
	virtual ~BlockHandler();
	void executeOwnNode(std::shared_ptr<GraphNode> ownNode);
	virtual void setData(PipelineData& data, const std::string& targetPort);

protected:
	std::string _blockName;
	virtual bool executeSpecialized() = 0;
	std::map<std::string, PipelineData&> _dataPerPort;

	const std::string _errorMessageBase = "Block execution failed due to incompatible input data dimension. ";

private:
	const HandlerMap& _allHandler;

};
