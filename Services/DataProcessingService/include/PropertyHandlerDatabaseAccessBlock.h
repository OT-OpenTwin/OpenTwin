#pragma once
#include "BufferBlockDatabaseAccess.h"
#include "OpenTwinCore/CoreTypes.h"
#include "EntityBlockDatabaseAccess.h"
#include "MeasurementCampaign.h"
#include "OpenTwinFoundation/BusinessLogicHandler.h"

#include <map>
#include <memory>

class PropertyHandlerDatabaseAccessBlock : public BusinessLogicHandler
{
public:
	void PerformUpdateIfRequired(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity,const std::string& sessionServiceURL, const std::string& modelServiceURL);
private:
	std::map<ot::UID, BufferBlockDatabaseAccess> _bufferedInformation;
	
	void Buffer(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity);
	void UpdateAllCampaignDependencies(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL);

	const MeasurementCampaign GetMeasurementCampaign(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL);
	void RequestPropertyUpdate(const std::string& modelServiceURL, ot::UIDList entityIDs, const std::string& propertiesAsJSON);
	void UpdateBuffer(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, std::list<std::string> msmdNames, std::list<std::string> parameterNames, std::list<std::string> quantityNames);
};
