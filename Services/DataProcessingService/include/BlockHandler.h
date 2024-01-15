#pragma once
#include <list>
#include "OTCore/Variable.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "GraphNode.h"
#include "GenericDataBlock.h"

class BlockHandler : public BusinessLogicHandler
{
public:
	using HandlerMap = std::map<std::shared_ptr<GraphNode>, std::shared_ptr<BlockHandler>>;
	BlockHandler(const HandlerMap& allHandler);
	virtual ~BlockHandler() {};
	void executeOwnNode(std::shared_ptr<GraphNode> ownNode);
	virtual void setData(std::list<GenericDataBlock>& data, const std::string& targetPort);	

protected:
	virtual bool executeSpecialized() = 0;
	std::map<std::string, std::list<GenericDataBlock>> _dataPerPort;

private:
	const HandlerMap& _allHandler;

};
