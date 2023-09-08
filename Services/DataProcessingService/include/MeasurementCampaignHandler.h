#pragma once
#include <vector>
#include <string>

#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "EntityMeasurementMetadata.h"

class MeasurementCampaignHandler : public BusinessLogicHandler
{
public:
	void ConnectToCollection(const std::string& collectionName, const std::string& projectName);
	std::vector<std::string> GetParameterList();
	std::vector<std::string> GetQuantityList();

private:
	std::vector<std::shared_ptr<EntityMeasurementMetadata>> _measurementMetadata;
};
