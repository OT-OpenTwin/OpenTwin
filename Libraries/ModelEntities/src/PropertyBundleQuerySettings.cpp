#include "PropertyBundleQuerySettings.h"
#include "PropertyHelper.h"
#include "OTCore/ComparisionSymbols.h"

void PropertyBundleQuerySettings::setProperties(EntityBase* _thisObject)
{
	EntityPropertiesInteger::createProperty(m_groupQuerySettings, m_propertyNbOfComparisions, 0, 0, m_maxNumberOfQueryDefinitions, "default", _thisObject->getProperties());
	std::list<std::string> comparisionSymbols = ot::ComparisionSymbols::g_comparators;
	comparisionSymbols.push_front("");
	for (int32_t i = 1; i <= m_maxNumberOfQueryDefinitions; i++)
	{
		std::string groupName = m_groupQueryDefinition + "_" + std::to_string(i);
		
		EntityPropertiesString::createProperty(groupName, m_propertyName, "", "default", _thisObject->getProperties());
		EntityPropertiesSelection::createProperty(groupName, m_propertyComparator, comparisionSymbols, "", "default", _thisObject->getProperties());
		EntityPropertiesString::createProperty(groupName, m_propertyValue, "", "default", _thisObject->getProperties());

		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(false);
		PropertyHelper::getStringProperty(_thisObject, m_propertyName, groupName)->setVisible(false);
		PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setVisible(false);
	}
}

bool PropertyBundleQuerySettings::updatePropertyVisibility(EntityBase* _thisObject)
{
	const int32_t numberOfQueries = PropertyHelper::getIntegerPropertyValue(_thisObject, m_propertyNbOfComparisions, m_groupQuerySettings);
	bool refresh = false;
	for (int32_t i = 1; i <= m_maxNumberOfQueryDefinitions; i++)
	{
		std::string groupName = m_groupQueryDefinition + "_" + std::to_string(i);
		const bool visible = i >= numberOfQueries;
		if (PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->getVisible() != visible)
		{
			PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(visible);
			PropertyHelper::getStringProperty(_thisObject, m_propertyName, groupName)->setVisible(visible);
			PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setVisible(visible);
			refresh = true;
		}
	}
	return refresh;
}


