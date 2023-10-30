#pragma once
#include <list>
#include "OpenTwinCore/Variable.h"
#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "GraphNode.h"


using genericDataBlock = std::list<ot::Variable>;

class BlockHandler : public BusinessLogicHandler
{
public:
	using HandlerMap = std::map<std::shared_ptr<GraphNode>, std::shared_ptr<BlockHandler>>;
	BlockHandler(const HandlerMap& allHandler);
	void executeOwnNode(std::shared_ptr<GraphNode> ownNode);
	virtual void setData(genericDataBlock& data, const std::string& targetPort);	

protected:
	virtual bool executeSpecialized() = 0;
	std::map<std::string, genericDataBlock> _dataPerPort;

private:
	const HandlerMap& _allHandler;

};
