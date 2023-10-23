#pragma once
#include <string>

#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "EntityBlock.h"
#include "OpenTwinCore/CoreTypes.h"
#include "OTGui/GraphicsPackage.h"

class BlockEntityHandler  : public BusinessLogicHandler
{
public:

	static BlockEntityHandler& GetInstance();
	std::string CreateBlockEntity(const std::string& editorName, const std::string& blockName, ot::Point2DD& position);
	void AddBlockConnection(const ot::GraphicsConnectionPackage::ConnectionInfo& connection);
	void RegisterBlockEntity(const std::string& key, std::function < std::shared_ptr<EntityBlock>()> factoryMethod);

private:
	const std::string _blockFolder = "Blocks";
	std::map < std::string, std::function<std::shared_ptr<EntityBlock>()>> _blockEntityFactories;

	BlockEntityHandler() {};
};
