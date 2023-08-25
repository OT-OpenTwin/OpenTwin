#pragma once
#include <string>

#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "EntityBlock.h"
#include "OpenTwinCore/CoreTypes.h"

class BlockEntityHandler  : public BusinessLogicHandler
{
public:
	std::shared_ptr <EntityBlock> CreateBlock(const std::string& editorName, const std::string& blockName, ot::UID itemID);

private:
	const std::string _blockFolder = "Blocks";
};
