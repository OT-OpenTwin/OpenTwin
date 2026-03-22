#include "OTModelEntities/PropertyBundleDataLakeQuery.h"
#include "OTCore/ComparisonSymbols.h"
#include "OTModelEntities/PropertyHelper.h"

void PropertyBundleDataLakeQuery::setProperties(EntityBase* _thisObject)
{
	std::list<std::string> comparators = ot::ComparisonSymbols::g_comparators;
	comparators.push_front("");

	EntityPropertiesProjectList* projectList = new EntityPropertiesProjectList(m_propertyNameProjectName);
	_thisObject->getProperties().createProperty(projectList, m_groupMetadataFilter);

	// Basic properties
	EntityPropertiesSelection* metadataProp = EntityPropertiesSelection::createProperty(m_groupMetadataFilter, m_propertyNameSeriesMetadata, { "" }, "", "default", _thisObject->getProperties());
	metadataProp->setAllowCustomValues(true);
	EntityPropertiesInteger::createProperty(m_groupMetadataFilter, m_propertyNumberOfQueries, 0, 0, m_maxNbOfQueries, "default", _thisObject->getProperties());
	EntityPropertiesInteger::createProperty(m_groupMetadataFilter, m_propertyNumberOfQueriesMetadataSeries, 0, 0, m_maxNbOfQueriesMetadata, "default", _thisObject->getProperties());

	// Quantity Settings
	EntityPropertiesSelection* groupQuantityProp = EntityPropertiesSelection::createProperty(m_groupQuantitySettings, m_propertyName, { "" }, "", "default", _thisObject->getProperties());
	groupQuantityProp->setAllowCustomValues(true);

	EntityPropertiesString* typeLabelProperty = new EntityPropertiesString();
	typeLabelProperty->setReadOnly(true);
	typeLabelProperty->setName(m_propertyDataType);
	typeLabelProperty->setGroup(m_groupQuantitySettings);
	typeLabelProperty->setValue("");
	_thisObject->getProperties().createProperty(typeLabelProperty, m_groupQuantitySettings);

	EntityPropertiesString* unitLabelProperty = new EntityPropertiesString();
	unitLabelProperty->setReadOnly(true);
	unitLabelProperty->setName(m_propertyUnit);
	unitLabelProperty->setGroup(m_groupQuantitySettings);
	unitLabelProperty->setValue("");
	_thisObject->getProperties().createProperty(unitLabelProperty, m_groupQuantitySettings);

	EntityPropertiesString::createProperty(m_groupQuantitySettings, m_propertyValue, "", "default", _thisObject->getProperties());
	EntityPropertiesSelection::createProperty(m_groupQuantitySettings, m_propertyComparator, comparators, comparators.front(), "default", _thisObject->getProperties());

	EntityPropertiesSelection::createProperty(m_groupTupleSettings, m_propertyTupleFormat, { }, "", "default", _thisObject->getProperties());
	PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleFormat, m_groupTupleSettings)->setVisible(false);
	EntityPropertiesSelection::createProperty(m_groupTupleSettings, m_propertyTupleTarget, { }, "", "default", _thisObject->getProperties());
	PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleTarget, m_groupTupleSettings)->setVisible(false);
	EntityPropertiesSelection::createProperty(m_groupTupleSettings, m_propertyTupleUnit, {}, "", "default", _thisObject->getProperties());
	PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleUnit, m_groupTupleSettings)->setVisible(false);

	EntityPropertiesBoolean::createProperty(m_groupQuerySettings, m_propertyOrder, true, "default", _thisObject->getProperties());


	for (uint32_t i = 1; i <= m_maxNbOfQueriesMetadata; i++)
	{
		const std::string groupName = m_groupSeriesMetadata + "_" + std::to_string(i);
		EntityPropertiesSelection::createProperty(groupName, m_propertyName, { "" }, "", "default", _thisObject->getProperties());
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(false);

		EntityPropertiesString::createProperty(groupName, m_propertyValue, "", "default", _thisObject->getProperties());
		PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setVisible(false);

		EntityPropertiesSelection::createProperty(groupName, m_propertyComparator, comparators, comparators.front(), "default", _thisObject->getProperties());
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(false);
	}
	//Add all query fields
	for (uint32_t i = 1; i <= m_maxNbOfQueries; i++)
	{
		const std::string groupName = m_groupQuerySettings + "_" + std::to_string(i);

		EntityPropertiesSelection::createProperty(groupName, m_propertyName, { "" }, "", "default", _thisObject->getProperties());
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(false);

		EntityPropertiesString* typeLabelProperty = new EntityPropertiesString();
		typeLabelProperty->setReadOnly(true);
		typeLabelProperty->setName(m_propertyDataType);
		typeLabelProperty->setGroup(groupName);
		typeLabelProperty->setValue("");
		_thisObject->getProperties().createProperty(typeLabelProperty, groupName);
		PropertyHelper::getStringProperty(_thisObject, m_propertyDataType, groupName)->setVisible(false);

		EntityPropertiesString* unitLabelProperty = new EntityPropertiesString();
		unitLabelProperty->setReadOnly(true);
		unitLabelProperty->setName(m_propertyUnit);
		unitLabelProperty->setGroup(groupName);
		unitLabelProperty->setValue("");
		_thisObject->getProperties().createProperty(unitLabelProperty, groupName);
		PropertyHelper::getStringProperty(_thisObject, m_propertyUnit, groupName)->setVisible(false);

		EntityPropertiesString::createProperty(groupName, m_propertyValue, "", "default", _thisObject->getProperties());
		PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setVisible(false);

		EntityPropertiesSelection::createProperty(groupName, m_propertyComparator, comparators, comparators.front(), "default", _thisObject->getProperties());
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(false);
	}
}

bool PropertyBundleDataLakeQuery::updatePropertyVisibility(EntityBase* _thisObject)
{

	EntityPropertiesInteger* numberOfQueriesProp = PropertyHelper::getIntegerProperty(_thisObject, m_propertyNumberOfQueries);
	bool requiresUpdate = numberOfQueriesProp->needsUpdate();
	if (requiresUpdate)
	{
		uint32_t numberVisible = static_cast<uint32_t>(numberOfQueriesProp->getValue());
		for (uint32_t i = 1; i <= m_maxNbOfQueries; i++)
		{
			const std::string groupName = m_groupQuerySettings + "_" + std::to_string(i);
			PropertyHelper::getStringProperty(_thisObject, m_propertyDataType, groupName)->setVisible(i <= numberVisible);
			PropertyHelper::getStringProperty(_thisObject, m_propertyUnit, groupName)->setVisible(i <= numberVisible);
			PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setVisible(i <= numberVisible);
			PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(i <= numberVisible);
			PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(i <= numberVisible);
		}
	}

	auto numberOfMetadataQueries = PropertyHelper::getIntegerProperty(_thisObject, m_propertyNumberOfQueriesMetadataSeries);
	bool metadataQueryRequiresUpdate = numberOfMetadataQueries->needsUpdate();
	if (metadataQueryRequiresUpdate)
	{
		uint32_t numberVisible = static_cast<uint32_t>(numberOfMetadataQueries->getValue());
		for (uint32_t i = 1; i <= m_maxNbOfQueriesMetadata; i++)
		{
			const std::string groupName = m_groupSeriesMetadata + "_" + std::to_string(i);
			PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setVisible(i <= numberVisible);
			PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(i <= numberVisible);
			PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(i <= numberVisible);
		}
	}
	return requiresUpdate;
}
