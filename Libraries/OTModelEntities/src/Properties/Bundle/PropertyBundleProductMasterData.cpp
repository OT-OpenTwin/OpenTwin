#include "OTModelEntities/Properties/Bundle/PropertyBundleProductMasterData.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesBase.h"
#include "OTCore/ProductMasterData/ZoneTag.h"
#include "OTModelEntities/Properties/PropertyHelper.h"

#include <cassert>

void PropertyBundleProductMasterData::setProperties(EntityBase * _thisObject)
{
	
	auto propertyRaw = EntityPropertiesBoolean::createProperty(m_groupName, getZoneTagPropertyName(ot::ZoneTag::RAW),false,"default", _thisObject->getProperties());
	propertyRaw->setReadOnly(true);
	auto propertyRefined = EntityPropertiesBoolean::createProperty(m_groupName, getZoneTagPropertyName(ot::ZoneTag::REFINED), false, "default", _thisObject->getProperties());
	propertyRefined->setReadOnly(true);
	auto propertyCompliant = EntityPropertiesBoolean::createProperty(m_groupName, getZoneTagPropertyName(ot::ZoneTag::COMPLIANT), false, "default", _thisObject->getProperties());
	propertyCompliant->setReadOnly(true);
}

std::string PropertyBundleProductMasterData::getZoneTagPropertyName(const ot::ZoneTag& _zoneTag) const
{
	ot::ZoneTagConverter zoneConverter;
	return m_propertyNameZoneTag + " " + zoneConverter.toString(_zoneTag);
}

bool PropertyBundleProductMasterData::isAlreadySet(EntityBase* _thisObject) const
{
	auto allGroups = _thisObject->getProperties().getListOfGroups();
	return std::find(allGroups.begin(), allGroups.end(), m_groupName) != allGroups.end();
}

void PropertyBundleProductMasterData::initialise(EntityBase* _thisObject, const ot::ProductMasterDataConfiguration& _config)
{

	if (!isAlreadySet(_thisObject))
	{
		setProperties(_thisObject);
	}
	setConfiguration(_thisObject, _config);
}

void PropertyBundleProductMasterData::setConfiguration(EntityBase* _thisObject, const ot::ProductMasterDataConfiguration& _config)
{
	assert(isAlreadySet(_thisObject));
	for (const auto zoneTag : _config.m_zoneTags)
	{
		const std::string zoneTagPropertyName = getZoneTagPropertyName(zoneTag);
		PropertyHelper::setBoolPropertyValue(true, _thisObject, zoneTagPropertyName, m_groupName);
	}
}

ot::ProductMasterDataConfiguration PropertyBundleProductMasterData::getSetProductMasterDataConfigurations(EntityBase* _thisObject)
{
	ot::ZoneTagConverter converter;
	ot::ProductMasterDataConfiguration configuration;
	for (auto zoneTag : converter.getAllZoneTags())
	{
		const std::string zoneTagPropertyName = getZoneTagPropertyName(zoneTag);
		bool tagIsSet =	PropertyHelper::getBoolPropertyValue(_thisObject, zoneTagPropertyName, m_groupName);
		if (tagIsSet)
		{
			configuration.m_zoneTags.insert(zoneTag);
		}
	}

	return configuration;
}

bool PropertyBundleProductMasterData::updatePropertyVisibility(EntityBase* _thisObject)
{
	return false;
}