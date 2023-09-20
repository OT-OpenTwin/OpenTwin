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
	
	EntityProperties UpdateAllCampaignDependencies(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL);
	EntityProperties UpdateSelectionProperties(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity);

	const MeasurementCampaign GetMeasurementCampaign(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL);
	void RequestPropertyUpdate(const std::string& modelServiceURL, ot::UIDList entityIDs, const std::string& propertiesAsJSON);
	
	
	void UpdateBuffer(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const MeasurementCampaign& campaignMetadata);
	void getSelectedValues(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, std::string& outQuantityValue, std::string& outParameter1Value, std::string& outParameter2Value, std::string& outParameter3Value);
	void getDataTypes(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, std::string& outQuantityType, std::string& outParameter1Type, std::string& outParameter2Type, std::string& outParameter3Type, bool& outParameter2Visible, bool& outParameter3Visible);
	void CreateUpdatedTypeProperty(EntityPropertiesBase* oldEntity, const std::string& value, EntityProperties& properties);
};
