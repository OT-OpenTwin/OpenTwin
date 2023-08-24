#include "BlockEntityHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "BlockDatabaseAccess.h"

std::shared_ptr<EntityBlock> BlockEntityHandler::CreateBlock(const std::string& editorName, const std::string& blockName, ot::UID itemID)
{
	std::shared_ptr<EntityBlock> blockEntity = nullptr;
	if (blockName == "Database access")
	{
		blockEntity = BlockDatabaseAccess::CreateBlockEntity();
	}
	else
	{
		assert(0);
	}

	assert(blockEntity != nullptr);
	
	std::string entName = CreateNewUniqueTopologyName(editorName, blockName);
	blockEntity->setName(entName);
	
	blockEntity->setEntityID(_modelComponent->createEntityUID());
	blockEntity->setOwningService(OT_INFO_SERVICE_TYPE_DataProcessingService);
	blockEntity->setBlockID(itemID);
	blockEntity->StoreToDataBase();

	return blockEntity;
}
