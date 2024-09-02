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
	
	std::list<ot::Connector> m_connectorsQuantity;
	std::list<ot::Connector> m_connectorsParameter;
	std::list<ot::Connector> m_connectorsMetadata;

	bool allInputsAvailable();
	std::list<DatasetDescription> createDatasets();

	void extractQuantityProperties(const ot::Connector& _connector, std::string& _outName, std::string& _outUnit, std::string& _outType);
	QuantityDescription* extractQuantityDescription(const ot::Connector& _connector, std::string& _outTypeName);
	std::list<std::shared_ptr<ParameterDescription>> createAllParameter();
	MetadataParameter extractParameter(const ot::Connector& _connector);
};
