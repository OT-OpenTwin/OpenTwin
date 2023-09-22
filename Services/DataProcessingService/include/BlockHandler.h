#pragma once
#include <list>
#include "OpenTwinCore/Variable.h"
#include "EntityBlock.h"
#include "OpenTwinFoundation/BusinessLogicHandler.h"

class BlockHandler : public BusinessLogicHandler
{
public:

	using genericDataBlock = std::map<std::string, std::list<ot::Variable>>;

	virtual genericDataBlock Execute(genericDataBlock& inputData) = 0;
	void addConnectorAssoziation(const std::string& otherCon, const std::string& inCon) { _ownToOtherConnector[inCon] = otherCon; };

protected:
	std::map<std::string, std::string> _ownToOtherConnector;
};
