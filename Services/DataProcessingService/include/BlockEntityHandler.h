#pragma once
#include <string>
#include <memory>

#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "EntityBlock.h"
#include "OpenTwinCore/CoreTypes.h"
#include "OpenTwinCore/FolderNames.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTGui/GraphicsPackage.h"


class BlockEntityHandler  : public BusinessLogicHandler
{
public:
	void CreateBlockEntity(const std::string& editorName, const std::string& blockName, ot::Point2DD& position);
	void AddBlockConnection(const std::list<ot::GraphicsConnectionCfg>& connections);
	void OrderUIToCreateBlockPicker();
	void UpdateBlockPosition(const std::string& blockID, ot::Point2DD& position);
	std::map<std::string, std::shared_ptr<EntityBlock>> findAllBlockEntitiesByBlockID();

private:
	const std::string _blockFolder = ot::FolderNames::BlockFolder;
	const std::string _packageName = "Data Processing";

	void InitSpecialisedBlockEntity(std::shared_ptr<EntityBlock> blockEntity);
	ot::GraphicsNewEditorPackage* BuildUpBlockPicker();
	bool connectorHasTypeOut(std::shared_ptr<EntityBlock> blockEntity, const std::string& connectorName);
};
