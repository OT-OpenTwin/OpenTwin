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
	metadataProp->setGroupChanges(true);
	EntityPropertiesInteger::createProperty(m_groupMetadataFilter, m_propertyNumberOfQueries, 0, 0, m_maxNbOfQueries, "default", _thisObject->getProperties());
	EntityPropertiesInteger::createProperty(m_groupMetadataFilter, m_propertyNumberOfQueriesMetadataSeries, 0, 0, m_maxNbOfQueriesMetadata, "default", _thisObject->getProperties());

	// Quantity Settings
	EntityPropertiesSelection* groupQuantityProp = EntityPropertiesSelection::createProperty(m_groupQuantitySettings, m_propertyName, { "" }, "", "default", _thisObject->getProperties());
	groupQuantityProp->setAllowCustomValues(true);
	groupQuantityProp->setGroupChanges(true);

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
	PropertyHelper::getStringProperty(_thisObject, m_propertyValue, m_groupQuantitySettings)->setGroupChanges(true);
	EntityPropertiesSelection::createProperty(m_groupQuantitySettings, m_propertyComparator, comparators, comparators.front(), "default", _thisObject->getProperties());
	PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, m_groupQuantitySettings)->setGroupChanges(true);

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
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setGroupChanges(true);

		EntityPropertiesString::createProperty(groupName, m_propertyValue, "", "default", _thisObject->getProperties());
		PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setVisible(false);
		PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setGroupChanges(true);

		EntityPropertiesSelection::createProperty(groupName, m_propertyComparator, comparators, comparators.front(), "default", _thisObject->getProperties());
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(false);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setGroupChanges(true);
	}
	//Add all query fields
	for (uint32_t i = 1; i <= m_maxNbOfQueries; i++)
	{
		const std::string groupName = m_groupQuerySettings + "_" + std::to_string(i);

		EntityPropertiesSelection::createProperty(groupName, m_propertyName, { "" }, "", "default", _thisObject->getProperties());
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(false);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setGroupChanges(true);

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
		PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setGroupChanges(true);

		EntityPropertiesSelection::createProperty(groupName, m_propertyComparator, comparators, comparators.front(), "default", _thisObject->getProperties());
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(false);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setGroupChanges(true);

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
	return requiresUpdate || metadataQueryRequiresUpdate;
}


std::list <std::string> PropertyBundleDataLakeQuery::getParameterOptions(EntityBase* _thisObject)
{
	const auto& options = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuerySettings + "_" + std::to_string(1))->getOptions();
	return { options.begin(), options.end() };
}

std::list <std::string> PropertyBundleDataLakeQuery::getQuantityOptions(EntityBase* _thisObject)
{
	const auto& options = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuantitySettings)->getOptions();
	return { options.begin(), options.end() };
}


std::string PropertyBundleDataLakeQuery::getSelectedProject(EntityBase* _thisObject)
{
	return PropertyHelper::getProjectPropertyValue(_thisObject, m_propertyNameProjectName);
}
std::string PropertyBundleDataLakeQuery::getSelectedSeries(EntityBase* _thisObject)
{
	return PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyNameSeriesMetadata, m_groupMetadataFilter);
}

bool PropertyBundleDataLakeQuery::updateOptions(EntityBase* _thisObject, MetadataCampaign& _campaign)
{
	bool refreshNecessary = false;
	bool projectChanged = PropertyHelper::getEntityProjectListProperty(_thisObject, m_propertyNameProjectName)->needsUpdate();

	const auto& allParameterByLabel = _campaign.getMetadataParameterByLabel();
	auto allQuantityByLabel = _campaign.getMetadataQuantitiesByLabel();

	if (projectChanged)
	{
		refreshNecessary = true;
		const std::list<MetadataSeries>& allSeries = _campaign.getSeriesMetadata();
		std::list<std::string> seriesNames;
		ot::JsonDocument overView;
		for (const MetadataSeries& series : allSeries)
		{
			seriesNames.push_back(series.getName());
			const ot::JsonDocument& metadata = series.getMetadata();
			ot::json::mergeObjects(overView, metadata, overView.GetAllocator());
		}
		std::list<std::string> allOptions;
		vectorize(overView, allOptions, "");
		for (uint32_t i = 1; i <= m_maxNbOfQueriesMetadata; i++)
		{
			const std::string groupName = m_groupSeriesMetadata + "_" + std::to_string(i);
			PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->resetOptions(allOptions);
		}



		seriesNames.push_front("");
		setNameOptions(_thisObject, seriesNames, m_groupMetadataFilter, m_propertyNameSeriesMetadata);

		//Could be used to filter the quantity and parameter options
		bool seriesChanged = PropertyHelper::getSelectionProperty(_thisObject, m_propertyNameSeriesMetadata)->needsUpdate();

		// First the label options for quantity and parameter. 
		// Available quantity could be filtered by the series selection, parameter selection by series and quantity selection.
		std::list<std::string> allParameterLabel;;
		for (const auto& parameterByLabel : allParameterByLabel)
		{
			allParameterLabel.push_back(parameterByLabel.first);
		}
		allParameterLabel.push_front("");
		for (uint32_t i = 1; i <= m_maxNbOfQueries; i++)
		{
			const std::string groupName = m_groupQuerySettings + "_" + std::to_string(i);
			setNameOptions(_thisObject,allParameterLabel, groupName, m_propertyName);
		}

		std::list<std::string> allQuantityLabel;;
		for (const auto& quantityByLabel : allQuantityByLabel)
		{
			allQuantityLabel.push_back(quantityByLabel.first);
		}
		allQuantityLabel.push_front("");
		setNameOptions(_thisObject, allQuantityLabel, m_groupQuantitySettings, m_propertyName);
	}

	bool refreshQuantityDescription = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuantitySettings)->needsUpdate();
	refreshNecessary |= refreshQuantityDescription;
	if (refreshQuantityDescription)
	{
		const std::string label = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyName, m_groupQuantitySettings);
		auto quantityByLabel =	allQuantityByLabel.find(label);
		if (quantityByLabel == allQuantityByLabel.end())
		{
			setValuePropertiesEmpty(_thisObject, m_groupQuantitySettings);
		}
		else
		{
			setValueProperties(_thisObject, m_groupQuantitySettings, quantityByLabel->second->m_tupleDescription);
		}
	}

	for (uint32_t i = 1; i <= m_maxNbOfQueries; i++)
	{
		const std::string groupName = m_groupQuerySettings + "_" + std::to_string(i);
		bool refreshParameterDescription = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->needsUpdate();
		refreshNecessary |= refreshParameterDescription;
		if (refreshParameterDescription)
		{
			const std::string label = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyName, groupName);
			auto parameterByLabel = allParameterByLabel.find(label);
			if (parameterByLabel == allParameterByLabel.end())
			{
				setValuePropertiesEmpty(_thisObject, groupName);
			}
			else
			{
				MetadataParameter* parameter = parameterByLabel->second;
				ot::TupleInstance single;
				single.setTupleElementDataTypes({ parameter->typeName });
				single.setTupleUnits({ parameter->unit });
				setValueProperties(_thisObject,groupName, single);
			}

		}
	}


	return refreshNecessary;
}


void PropertyBundleDataLakeQuery::setNameOptions(EntityBase* _thisObject, const std::list<std::string>& _options, const std::string& _group, const std::string& _name)
{
	PropertyHelper::getSelectionProperty(_thisObject, _name, _group)->resetOptions(_options);
}
void PropertyBundleDataLakeQuery::setValueProperties(EntityBase* _thisObject, const std::string& _groupName, const ot::TupleInstance& _tupleInstance)
{
	PropertyHelper::getStringProperty(_thisObject, m_propertyDataType, _groupName)->setValue(_tupleInstance.getTupleElementDataTypes().front());
	PropertyHelper::getStringProperty(_thisObject, m_propertyUnit, _groupName)->setValue(_tupleInstance.getTupleUnits().front());
}
void PropertyBundleDataLakeQuery::setValuePropertiesEmpty(EntityBase* _thisObject, const std::string& _groupName)
{
	PropertyHelper::getStringProperty(_thisObject, m_propertyDataType, _groupName)->setValue("");
	PropertyHelper::getStringProperty(_thisObject, m_propertyUnit, _groupName)->setValue("");
}

void PropertyBundleDataLakeQuery::vectorize(const ot::JsonValue& _value, std::list<std::string>& _allEntries, const std::string& _nameBase)
{
	if (_value.IsObject())
	{
		std::string separator = _nameBase.empty() ? "" : "~";
		for (auto& element : _value.GetObject())
		{
			std::string name = _nameBase + separator + element.name.GetString();
			ot::String::removeControlCharacters(name);
			_allEntries.push_back(name);
			if (element.value.IsObject())
			{
				vectorize(element.value, _allEntries, name);
			}
		}
	}
}

ot::ValueComparisonDescription PropertyBundleDataLakeQuery::getQuantityQuery(EntityBase* _thisObject) const
{
	const std::string dataType = PropertyHelper::getStringPropertyValue(_thisObject, m_propertyDataType, m_groupQuantitySettings);
	const std::string unit = PropertyHelper::getStringPropertyValue(_thisObject, m_propertyUnit, m_groupQuantitySettings);
	const std::string value = PropertyHelper::getStringPropertyValue(_thisObject, m_propertyValue, m_groupQuantitySettings);
	const std::string name = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyName, m_groupQuantitySettings);
	const std::string comparator = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyComparator, m_groupQuantitySettings);
	const ot::ValueComparisonDescription valueComparisonDefinition(name, comparator, value, dataType, unit);
	return valueComparisonDefinition;
}

std::list<ot::ValueComparisonDescription> PropertyBundleDataLakeQuery::getParameterQueries(EntityBase* _thisObject) const
{
	std::list<ot::ValueComparisonDescription> valueComparisonDefinitions;
	for (uint32_t i = 1; i <= m_maxNbOfQueries; i++)
	{
		const std::string groupName = m_groupQuerySettings + "_" + std::to_string(i);
		const std::string dataType = PropertyHelper::getStringPropertyValue(_thisObject, m_propertyDataType, groupName);
		const std::string unit = PropertyHelper::getStringPropertyValue(_thisObject, m_propertyUnit, groupName);
		const std::string value = PropertyHelper::getStringPropertyValue(_thisObject, m_propertyValue, groupName);
		const std::string name = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyName, groupName);
		const std::string comparator = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyComparator, groupName);
		const ot::ValueComparisonDescription valueComparisonDefinition(name, comparator, value, dataType, unit);
		valueComparisonDefinitions.push_back(valueComparisonDefinition);
	}
	return valueComparisonDefinitions;
}

std::list<ot::ValueComparisonDescription> PropertyBundleDataLakeQuery::getMetadataQueries(EntityBase* _thisObject) const
{
	std::list<ot::ValueComparisonDescription> definitions;
	for (uint32_t index = 1; index < m_maxNbOfQueriesMetadata; index++)
	{
		const std::string groupName = m_groupSeriesMetadata + "_" + std::to_string(index);
		
		const std::string name = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyName, groupName);
		const std::string comparator = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyComparator, groupName);
		const std::string value = PropertyHelper::getStringPropertyValue(_thisObject, m_propertyValue, groupName);
		ot::ValueComparisonDescription definition(name, comparator, value, "", "");
		definitions.push_back(definition);
	}
	return definitions;
}
