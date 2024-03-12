#pragma once
#include <list>
#include "OTCore/Variable.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "GraphNode.h"
#include "OTCore/GenericDataStruct.h"

class BlockHandler : public BusinessLogicHandler
{
public:
	using HandlerMap = std::map<std::shared_ptr<GraphNode>, std::shared_ptr<BlockHandler>>;
	using GenericDataList = std::list<std::shared_ptr<ot::GenericDataStruct>>;
	BlockHandler(const HandlerMap& allHandler);
	virtual ~BlockHandler();
	void executeOwnNode(std::shared_ptr<GraphNode> ownNode);
	virtual void setData(GenericDataList& data, const std::string& targetPort);

protected:
	virtual bool executeSpecialized() = 0;
	std::map<std::string,GenericDataList> _dataPerPort;

	const std::string _errorMessageBase = "Block execution failed due to incompatible input data dimension. ";

private:
	const HandlerMap& _allHandler;

};
