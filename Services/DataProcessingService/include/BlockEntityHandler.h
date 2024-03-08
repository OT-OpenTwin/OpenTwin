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

class BlockEntityHandler  : public BusinessLogicHandler
{
public:
	void CreateBlockEntity(const std::string& editorName, const std::string& blockName, ot::Point2DD& position);
	void AddBlockConnection(const std::list<ot::GraphicsConnectionCfg>& connections);
	void OrderUIToCreateBlockPicker();
	void UpdateBlockPosition(const ot::UID& blockID, ot::Point2DD& position, ClassFactory *classFactory);
	std::map<ot::UID, std::shared_ptr<EntityBlock>> findAllBlockEntitiesByBlockID();

	static std::string getQueryForRangeSelection() { return "Range"; }
private:
	const std::string _blockFolder = ot::FolderNames::BlockFolder;
	const std::string _connectionFolder = "Connections";
	const std::string _packageName = "Data Processing";

	void InitSpecialisedBlockEntity(std::shared_ptr<EntityBlock> blockEntity);
	ot::GraphicsNewEditorPackage* BuildUpBlockPicker();
	bool connectorHasTypeOut(std::shared_ptr<EntityBlock> blockEntity, const std::string& connectorName);
};
