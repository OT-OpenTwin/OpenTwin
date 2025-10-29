// @otlicense

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
		
		EntityPropertiesSelection::createProperty(groupName, m_propertyName, m_selectionOptions,"", "default", _thisObject->getProperties());
		EntityPropertiesSelection::createProperty(groupName, m_propertyComparator, comparisionSymbols, "", "default", _thisObject->getProperties());
		EntityPropertiesString::createProperty(groupName, m_propertyValue, "", "default", _thisObject->getProperties());

		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(false);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(false);
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
		const bool visible = i <= numberOfQueries;
		if (PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->getVisible() != visible)
		{
			PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(visible);
			PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(visible);
			PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setVisible(visible);
			refresh = true;
		}
	}
	return refresh;
}

void PropertyBundleQuerySettings::setQueryDefinitions(const std::list<std::string>& _queryOptions)
{
	m_maxNumberOfQueryDefinitions = static_cast<uint32_t>(_queryOptions.size());
	m_selectionOptions = _queryOptions;
}

void PropertyBundleQuerySettings::reload(EntityBase* _thisObject)
{
	std::string groupName = m_groupQueryDefinition + "_" + std::to_string(1);
	const std::vector<std::string>& options =	PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->getOptions();
	m_selectionOptions = { options.begin(), options.end() };
	m_maxNumberOfQueryDefinitions =	m_selectionOptions.size();
}

std::list<ValueComparisionDefinition> PropertyBundleQuerySettings::getValueComparisionDefinitions(EntityBase* _thisObject)
{
	std::list<ValueComparisionDefinition> valueDefinitions;
	for (int32_t i = 1; i <= m_maxNumberOfQueryDefinitions; i++)
	{
		std::string groupName = m_groupQueryDefinition + "_" + std::to_string(i);
		if (PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->getVisible() == true)
		{
			std::string comparator = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyComparator, groupName);
			std::string name = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyName, groupName);
			std::string value = PropertyHelper::getStringPropertyValue(_thisObject, m_propertyValue, groupName);
			
			ValueComparisionDefinition definition(name, comparator, value, "", "");
			valueDefinitions.push_back(definition);
		}
	}

	return valueDefinitions;
}

bool PropertyBundleQuerySettings::requiresUpdate(EntityBase* _thisObject)
{
	const int32_t numberOfQueries = PropertyHelper::getIntegerPropertyValue(_thisObject, m_propertyNbOfComparisions, m_groupQuerySettings);
	bool updateRequired = false;
	for (int32_t i = 1; i <= m_maxNumberOfQueryDefinitions; i++)
	{
		std::string groupName = m_groupQueryDefinition + "_" + std::to_string(i);
		if (PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->getVisible())
		{
			auto selectedComparator =	PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName);
			auto queryValue = PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName);

			updateRequired |=
				selectedComparator->needsUpdate() | //If a query could be performed we check now if the properties have changed
				queryValue->needsUpdate() |
				PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->needsUpdate();
		}
	}
	return updateRequired;
}

EntityPropertiesInteger* PropertyBundleQuerySettings::getNumberOfQueriesProperty(EntityBase* _thisObject)
{
	return PropertyHelper::getIntegerProperty(_thisObject, m_propertyNbOfComparisions, m_groupQuerySettings);
}

void PropertyBundleQuerySettings::updateQuerySettings(EntityBase* _thisObject, const std::list<std::string>& _queryOptions)
{

	if (m_selectionOptions != _queryOptions)
	{
		std::list<std::string> comparisionSymbols = ot::ComparisionSymbols::g_comparators;
		m_selectionOptions = _queryOptions;

		PropertyHelper::getIntegerProperty(_thisObject, m_propertyNbOfComparisions, m_groupQuerySettings)->setMax(_queryOptions.size());

		for (int32_t i = 1; i <= m_maxNumberOfQueryDefinitions; i++)
		{
			std::string groupName = m_groupQueryDefinition + "_" + std::to_string(i);
			PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->resetOptions(m_selectionOptions);
		}
		if (_queryOptions.size() > m_maxNumberOfQueryDefinitions)
		{
			for (int i = m_maxNumberOfQueryDefinitions + 1; i <= _queryOptions.size(); i++)
			{
				std::string groupName = m_groupQueryDefinition + "_" + std::to_string(i);

				EntityPropertiesSelection::createProperty(groupName, m_propertyName, m_selectionOptions, "", "default", _thisObject->getProperties());
				EntityPropertiesSelection::createProperty(groupName, m_propertyComparator, comparisionSymbols, "", "default", _thisObject->getProperties());
				EntityPropertiesString::createProperty(groupName, m_propertyValue, "", "default", _thisObject->getProperties());

				PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(false);
				PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(false);
				PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setVisible(false);
			}

			m_maxNumberOfQueryDefinitions = _queryOptions.size();
		}
		else if(_queryOptions.size() < m_maxNumberOfQueryDefinitions)
		{
			for (int i = m_maxNumberOfQueryDefinitions; i > _queryOptions.size(); i--)
			{
				std::string groupName = m_groupQueryDefinition + "_" + std::to_string(i);
				_thisObject->getProperties().deleteProperty(m_propertyName, groupName);
				_thisObject->getProperties().deleteProperty(m_propertyComparator, groupName);
				_thisObject->getProperties().deleteProperty(m_propertyValue, groupName);
			}
			m_maxNumberOfQueryDefinitions = _queryOptions.size();
		}
	}
}

