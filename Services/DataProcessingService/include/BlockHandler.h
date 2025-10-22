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

	BlockHandler(EntityBlock* _blockEntity, const HandlerMap& _allHandler);
	virtual ~BlockHandler() = default;
	void executeOwnNode(std::shared_ptr<GraphNode> _ownNode);
	virtual void setData(PipelineData* _data, const std::string& _targetPort);
	virtual std::string getBlockType() const = 0;

protected:
	std::string m_blockName;
	virtual bool executeSpecialized() = 0;
	std::map<std::string, PipelineData*> m_dataPerPort;


	const std::string getErrorIncompatibleDataFormat()
	{
		return m_blockName + " failed to execute due to incompatible input data dimension.";
	}

	const std::string getErrorDataPipelineNllptr()
	{
		return m_blockName + " failed to execute  because no data was set in the incomming pipeline.";
	}

private:
	const HandlerMap& m_allHandler;

};
