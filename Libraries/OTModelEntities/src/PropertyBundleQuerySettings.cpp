// @otlicense
// File: PropertyBundleQuerySettings.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "OTModelEntities/PropertyBundleQuerySettings.h"
#include "OTModelEntities/PropertyHelper.h"
#include "OTCore/ComparisonSymbols.h"

void ot::PropertyBundleQuerySettings::setProperties(EntityBase* _entity)
{
	EntityPropertiesInteger::createProperty(m_groupQuerySettings, m_propertyNbOfComparisions, 0, 0, m_maxNumberOfQueryDefinitions, "default", _entity->getProperties());
	
	for (uint32_t i = 1; i <= m_maxNumberOfQueryDefinitions; i++)
	{
		std::string groupName = getGroupName(i);
		
		createPropertiesForIndex(_entity, i);
		setPropertiesVisible(_entity, i, false);
	}
}

bool ot::PropertyBundleQuerySettings::updatePropertyVisibility(EntityBase* _entity)
{
	const int32_t numberOfQueries = PropertyHelper::getIntegerPropertyValue(_entity, m_propertyNbOfComparisions, m_groupQuerySettings);
	bool refresh = false;
	for (uint32_t i = 1; i <= m_maxNumberOfQueryDefinitions; i++)
	{
		std::string groupName = getGroupName(i);
		const bool visible = ((int32_t)i <= numberOfQueries);
		if (PropertyHelper::getSelectionProperty(_entity, m_propertyComparator, groupName)->getVisible() != visible)
		{
			setPropertiesVisible(_entity, i, visible);
			refresh = true;
		}
	}
	return refresh;
}

void ot::PropertyBundleQuerySettings::setQueryDefinitions(const std::list<std::string>& _queryOptions)
{
	m_maxNumberOfQueryDefinitions = static_cast<uint32_t>(_queryOptions.size());
	m_nameSelectionOptions = _queryOptions;
}

void ot::PropertyBundleQuerySettings::reload(EntityBase* _entity)
{
	std::string groupName = getGroupName(1);
	const std::vector<std::string>& options = PropertyHelper::getSelectionProperty(_entity, m_propertyName, groupName)->getOptions();
	m_nameSelectionOptions = { options.begin(), options.end() };
	m_maxNumberOfQueryDefinitions =	(uint32_t)m_nameSelectionOptions.size();
}

std::list<ot::ValueComparisonDescription> ot::PropertyBundleQuerySettings::getValueComparisonDefinitions(const EntityBase* _entity) const
{
	std::list<ot::ValueComparisonDescription> valueDefinitions;
	for (uint32_t i = 1; i <= m_maxNumberOfQueryDefinitions; i++)
	{
		const std::string groupName = getGroupName(i);
		if (PropertyHelper::getSelectionProperty(_entity, m_propertyComparator, groupName)->getVisible() == true)
		{
			valueDefinitions.push_back(getValueComparisonDefinition(_entity, groupName));
		}
	}

	return valueDefinitions;
}

bool ot::PropertyBundleQuerySettings::needsUpdate(const EntityBase* _entity) const
{
	auto numberOfQueriesProp = PropertyHelper::getIntegerProperty(_entity, m_propertyNbOfComparisions, m_groupQuerySettings);
	bool updateRequired = numberOfQueriesProp->needsUpdate();

	for (uint32_t i = 1; i <= m_maxNumberOfQueryDefinitions; i++)
	{
		updateRequired |= needsUpdate(_entity, i);
		if (updateRequired)
		{
			break;
		}
	}
	return updateRequired;
}

int32_t ot::PropertyBundleQuerySettings::getNumberOfQueries(const EntityBase* _entity) const
{
	return PropertyHelper::getIntegerPropertyValue(_entity, m_propertyNbOfComparisions, m_groupQuerySettings);
}

void ot::PropertyBundleQuerySettings::updateQuerySettings(EntityBase* _entity, const std::list<std::string>& _queryOptions)
{

	if (m_nameSelectionOptions != _queryOptions)
	{
		std::list<std::string> comparisonSymbols = ot::ComparisonSymbols::g_comparators;
		m_nameSelectionOptions = _queryOptions;

		auto* comparisonsNbProp = PropertyHelper::getIntegerProperty(_entity, m_propertyNbOfComparisions, m_groupQuerySettings);
		OTAssertNullptr(comparisonsNbProp);
		comparisonsNbProp->setMax((long)_queryOptions.size());

		resetNameOptions(_entity);

		for (uint32_t i = 1; i <= m_maxNumberOfQueryDefinitions; i++)
		{
			
		}
		if (_queryOptions.size() > m_maxNumberOfQueryDefinitions)
		{
			for (int i = m_maxNumberOfQueryDefinitions + 1; i <= _queryOptions.size(); i++)
			{
				setPropertiesVisible(_entity, i, false);
			}

			m_maxNumberOfQueryDefinitions = static_cast<uint32_t>(_queryOptions.size());
		}
		else if(_queryOptions.size() < m_maxNumberOfQueryDefinitions)
		{
			for (int i = m_maxNumberOfQueryDefinitions; i > _queryOptions.size(); i--)
			{
				deletePropertiesForIndex(_entity, i);
			}
			m_maxNumberOfQueryDefinitions = static_cast<uint32_t>(_queryOptions.size());
		}
	}
}

void ot::PropertyBundleQuerySettings::createPropertiesForIndex(EntityBase* _entity, uint32_t _queryIndex)
{
	const std::string groupName = getGroupName(_queryIndex);

	std::list<std::string> comparisonSymbols = ot::ComparisonSymbols::g_comparators;
	comparisonSymbols.push_front("");
	EntityPropertiesSelection::createProperty(groupName, m_propertyName, m_nameSelectionOptions, "", "default", _entity->getProperties());
	EntityPropertiesSelection::createProperty(groupName, m_propertyComparator, comparisonSymbols, "", "default", _entity->getProperties());
	EntityPropertiesString::createProperty(groupName, m_propertyValue, "", "default", _entity->getProperties());
}

void ot::PropertyBundleQuerySettings::deletePropertiesForIndex(EntityBase* _entity, uint32_t _queryIndex)
{
	const std::string groupName = getGroupName(_queryIndex);

	_entity->getProperties().deleteProperty(m_propertyName, groupName);
	_entity->getProperties().deleteProperty(m_propertyComparator, groupName);
	_entity->getProperties().deleteProperty(m_propertyValue, groupName);
}

void ot::PropertyBundleQuerySettings::setPropertiesVisible(EntityBase* _entity, uint32_t _queryIndex, bool _visible)
{
	const std::string groupName = getGroupName(_queryIndex);

	EntityPropertiesBase* prop = PropertyHelper::getSelectionProperty(_entity, m_propertyComparator, groupName);
	OTAssertNullptr(prop);
	prop->setVisible(_visible);

	prop = PropertyHelper::getSelectionProperty(_entity, m_propertyName, groupName);
	OTAssertNullptr(prop);
	prop->setVisible(_visible);

	prop = PropertyHelper::getStringProperty(_entity, m_propertyValue, groupName);
	OTAssertNullptr(prop);
	prop->setVisible(_visible);
}

bool ot::PropertyBundleQuerySettings::needsUpdate(const EntityBase* _entity, uint32_t _queryIndex) const
{
	const std::string groupName = getGroupName(_queryIndex);

	bool updateRequired = false;

	const auto comparatorProperty = PropertyHelper::getSelectionProperty(_entity, m_propertyComparator, groupName);
	OTAssertNullptr(comparatorProperty);

	if (comparatorProperty->getVisible() && !updateRequired)
	{
		const auto queryValue = PropertyHelper::getStringProperty(_entity, m_propertyValue, groupName);
		OTAssertNullptr(queryValue);
		const auto nameProperty = PropertyHelper::getSelectionProperty(_entity, m_propertyName, groupName);
		OTAssertNullptr(nameProperty);

		updateRequired |=
			comparatorProperty->needsUpdate() || // If a query could be performed we check now if the properties have changed
			queryValue->needsUpdate()         ||
			nameProperty->needsUpdate()
		;
	}

	return updateRequired;
}

ot::ValueComparisonDescription ot::PropertyBundleQuerySettings::getValueComparisonDefinition(const EntityBase* _entity, const std::string& _groupName) const
{	
	const std::string comparator = PropertyHelper::getSelectionPropertyValue(_entity, m_propertyComparator, _groupName);
	const std::string name = PropertyHelper::getSelectionPropertyValue(_entity, m_propertyName, _groupName);
	const std::string value = PropertyHelper::getStringPropertyValue(_entity, m_propertyValue, _groupName);

	ValueComparisonDescription definition(name, comparator, value, "", "");
	return definition;
}

void ot::PropertyBundleQuerySettings::resetNameOptions(EntityBase* _entity)
{
	for (uint32_t i = 1; i <= m_maxNumberOfQueryDefinitions; i++)
	{
		const std::string groupName = getGroupName(i);

		auto* nameProp = PropertyHelper::getSelectionProperty(_entity, m_propertyName, groupName);
		OTAssertNullptr(nameProp);

		nameProp->resetOptions(m_nameSelectionOptions);
	}
}