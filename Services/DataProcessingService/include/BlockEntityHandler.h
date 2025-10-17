#pragma once
#include <string>
#include <memory>

#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "EntityBlock.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/FolderNames.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "EntityBlockConnection.h"

class BlockEntityHandler : public BusinessLogicHandler
{
public:
	void createBlockEntity(const std::string& editorName, const std::string& blockName, const ot::Point2DD& position);
	void addBlockConnection(const std::list<ot::GraphicsConnectionCfg>& connections, const std::string& _baseFolderName);
	void orderUIToCreateBlockPicker();
	std::map<ot::UID, std::shared_ptr<EntityBlock>> findAllBlockEntitiesByBlockID(const std::string& _folderName);
	std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> findAllEntityBlockConnections(const std::string& _folderName);

private:
	const std::string m_blockFolder = "Blocks";
	const std::string m_connectionFolder = "Connections";
	const std::string m_packageName = "Data Processing";
	const std::string m_connectionsFolder = "Connections";

	void initSpecialisedBlockEntity(std::shared_ptr<EntityBlock> blockEntity);
	ot::GraphicsPickerCollectionPackage BuildUpBlockPicker();
	bool connectorHasTypeOut(std::shared_ptr<EntityBlock> blockEntity, const std::string& connectorName);
};
