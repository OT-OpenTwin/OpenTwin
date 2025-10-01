#pragma once
#include <string>
#include <memory>

#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "EntityBlock.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/FolderNames.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTGui/GraphicsPackage.h"

class ClassFactory;

class BlockEntityHandler : public BusinessLogicHandler
{
public:
	void CreateBlockEntity(const std::string& editorName, const std::string& blockName, ot::Point2DD& position);
	void AddBlockConnection(const std::list<ot::GraphicsConnectionCfg>& connections, const std::string& _baseFolderName);
	void OrderUIToCreateBlockPicker();
	void UpdateBlockPosition(const ot::UID& blockID, const ot::Point2DD& position, ClassFactory *classFactory);
	std::map<ot::UID, std::shared_ptr<EntityBlock>> findAllBlockEntitiesByBlockID(const std::string& _folderName);

private:
	const std::string _blockFolder = "Blocks";
	const std::string _connectionFolder = "Connections";
	const std::string _packageName = "Data Processing";

	void InitSpecialisedBlockEntity(std::shared_ptr<EntityBlock> blockEntity);
	ot::GraphicsNewEditorPackage* BuildUpBlockPicker();
	bool connectorHasTypeOut(std::shared_ptr<EntityBlock> blockEntity, const std::string& connectorName);
};
