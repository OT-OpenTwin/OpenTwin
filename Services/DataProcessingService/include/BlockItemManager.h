#pragma once
#include <map>
#include "OTGui/GraphicsPackage.h"
#include "OpenTwinCore/Point2D.h"

class BlockItemManager
{
public:
	void OrderUIToCreateBlockPicker();
	OT_rJSON_doc CreateBlockItem(const std::string blockName, ot::UID blockID, ot::Point2DD& position);
	ot::GraphicsItemCfg* GetBlockConfiguration(const std::string& blockName);
private:
	const std::string _packageName = "Data Processing";

	ot::GraphicsNewEditorPackage* BuildUpBlockPicker();
	std::shared_ptr<ot::GraphicsScenePackage> GetBlockConfiguration(const std::string blockName, ot::UID blockID, ot::Point2DD& position);
};
