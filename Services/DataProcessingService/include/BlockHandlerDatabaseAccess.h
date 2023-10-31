#pragma once
#include "BlockHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "Document/DocumentAccess.h"
#include "MeasurementCampaign.h"


class BlockHandlerDatabaseAccess : public BlockHandler
{
public:
	BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap);
	~BlockHandlerDatabaseAccess();
	
	bool executeSpecialized() override;	

private:
	DataStorageAPI::DocumentAccess* _dataStorageAccess = nullptr;
	genericDataBlock _output;
	bool _isValid = true;
	std::string _quantityConnectorName;
	std::string _parameterConnectorName;

	std::string _queryString;
	std::string _projectionString;

	const MeasurementCampaign getMeasurementCampaign(EntityBlockDatabaseAccess* dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL);
};
