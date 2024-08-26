#pragma once
#include "BlockHandler.h"
#include "EntityBlockStorage.h"
#include "ResultCollectionExtender.h"

class BlockHandlerStorage : public BlockHandler
{
public:
	BlockHandlerStorage(EntityBlockStorage* blockEntity, const HandlerMap& handlerMap);
	~BlockHandlerStorage();

	bool executeSpecialized() override;

private:
	ResultCollectionExtender* m_resultCollectionExtender = nullptr;
};
