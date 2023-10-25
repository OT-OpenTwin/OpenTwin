#pragma once
#include <string>
#include <memory>

#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "EntityBlock.h"
#include "OpenTwinCore/CoreTypes.h"
#include "OTGui/GraphicsPackage.h"

class BlockEntityHandler  : public BusinessLogicHandler
{
public:
	void CreateBlockEntity(const std::string& editorName, const std::string& blockName, ot::Point2DD& position);
	void AddBlockConnection(const std::list<ot::GraphicsConnectionPackage::ConnectionInfo>& connections);
	void OrderUIToCreateBlockPicker();

private:
	const std::string _blockFolder = "Blocks";
	const std::string _packageName = "Data Processing";

	void InitSpecialisedBlockEntity(std::shared_ptr<EntityBlock> blockEntity);
	ot::GraphicsNewEditorPackage* BuildUpBlockPicker();
};
