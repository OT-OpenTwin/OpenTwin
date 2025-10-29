// @otlicense

#pragma once
#include "BlockHandler.h"
#include "EntityBlockStorage.h"
#include "ResultCollectionExtender.h"
#include "MetadataParameter.h"

class BlockHandlerStorage : public BlockHandler
{
public:
	BlockHandlerStorage(EntityBlockStorage* blockEntity, const HandlerMap& handlerMap);
	~BlockHandlerStorage() = default;

	bool executeSpecialized() override;

	// Inherited via BlockHandler
	std::string getBlockType() const override;
private:
	EntityBlockStorage* m_blockEntityStorage;
	
	std::list<std::string> m_allDataInputs;
	std::string m_seriesMetadataInput;
	bool m_createPlot;
	std::string m_plotName;


	bool allInputsAvailable();
	std::list<DatasetDescription> createDatasets();


};
