#pragma once
#include <vector>
#include <string>

#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "EntityMetadataSeries.h"
#include "OTCore/Variable.h"

class MeasurementCampaignHandler : public BusinessLogicHandler
{
public:
	void ConnectToCollection(const std::string& collectionName, const std::string& projectName);
	std::vector<std::string> GetParameterList();
	std::vector<std::string> GetQuantityList();

private:

	std::list<ot::EntityInformation> getMSMDEntityInformation(const std::string& collectionName, const std::string& projectName);
	void CollectMetaInformation(std::vector<std::shared_ptr<EntityMetadataSeries>>& measurementMetadata);
};
