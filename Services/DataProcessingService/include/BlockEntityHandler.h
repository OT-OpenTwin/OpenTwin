#pragma once
#include <string>

#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "EntityBlock.h"
#include "OpenTwinCore/CoreTypes.h"

class BlockEntityHandler  : public BusinessLogicHandler
{
public:

	static BlockEntityHandler& GetInstance();
	ot::UID CreateBlockEntity(const std::string& editorName, const std::string& blockName, ot::Point2DD& position);
	void AddBlockConnection(ot::UID idOrigin, ot::UID idDestination, const std::string& connectorOrigin, const std::string& connectorDest);
	void RegisterBlockEntity(const std::string& key, std::function < std::shared_ptr<EntityBlock>()> factoryMethod);

private:
	const std::string _blockFolder = "Blocks";
	std::map < std::string, std::function<std::shared_ptr<EntityBlock>()>> _blockEntityFactories;

	BlockEntityHandler() {};
};
