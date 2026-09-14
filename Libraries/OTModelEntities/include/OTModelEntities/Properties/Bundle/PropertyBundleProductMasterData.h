#pragma once
//@otlicense
#include "OTModelEntities/Properties/Bundle/PropertyBundle.h"
#include "OTCore/ProductMasterData/ProductMasterDataConfiguration.h"

#include "OTModelEntities/ModelEntitiesAPI.h"

class OT_MODELENTITIES_API_EXPORT PropertyBundleProductMasterData : public PropertyBundle
{
public:	
	PropertyBundleProductMasterData() {};
	virtual ~PropertyBundleProductMasterData() {};

	bool isAlreadySet(EntityBase* _thisObject) const;
	void initialise(EntityBase* _thisObject, const ot::ProductMasterDataConfiguration& _config);
	void setConfiguration(EntityBase* _thisObject, const ot::ProductMasterDataConfiguration& _config);
	ot::ProductMasterDataConfiguration getSetProductMasterDataConfigurations(EntityBase* _thisObject);
	bool updatePropertyVisibility(EntityBase* _thisObject) override;

private:
	std::string m_groupName = "Product Master Data";
	std::string m_propertyNameZoneTag = "Zone Tag";
	
	void setProperties(EntityBase * _thisObject) override;
	std::string getZoneTagPropertyName(const ot::ZoneTag& _zoneTag) const;
};



