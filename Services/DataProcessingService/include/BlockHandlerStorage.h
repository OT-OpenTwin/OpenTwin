#pragma once
#include "BlockHandler.h"
#include "EntityBlockStorage.h"
#include "ResultCollectionExtender.h"
#include "MetadataParameter.h"

class BlockHandlerStorage : public BlockHandler
{
public:
	BlockHandlerStorage(EntityBlockStorage* blockEntity, const HandlerMap& handlerMap);
	~BlockHandlerStorage();

	bool executeSpecialized() override;

private:
	EntityBlockStorage* m_blockEntityStorage;
	
	std::list<std::string> m_allInputs;


	bool allInputsAvailable();
	std::list<DatasetDescription> createDatasets();
};
