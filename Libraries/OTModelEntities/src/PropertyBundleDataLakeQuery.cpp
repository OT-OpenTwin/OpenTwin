#include "OTModelEntities/PropertyBundleDataLakeQuery.h"
#include "OTCore/ComparisonSymbols.h"
#include "OTModelEntities/PropertyHelper.h"
#include "OTCore/DataFilter/RegexHelper.h"

void PropertyBundleDataLakeQuery::setProperties(EntityBase* _thisObject)
{
	std::list<std::string> comparators = ot::ComparisonSymbols::g_comparators;
	comparators.push_front("");

	EntityPropertiesProjectList* projectList = new EntityPropertiesProjectList(m_propertyNameProjectName);
	_thisObject->getProperties().createProperty(projectList, m_groupMetadataFilter);

	// Basic properties
	EntityPropertiesSelection* metadataProp = EntityPropertiesSelection::createProperty(m_groupMetadataFilter, m_propertyNameSeriesMetadata, { "" }, "", "default", _thisObject->getProperties());
	metadataProp->setAllowCustomValues(true);
	//metadataProp->setGroupChanges(true);
	EntityPropertiesInteger::createProperty(m_groupMetadataFilter, m_propertyNumberOfQueries, 0, 0, m_maxNbOfQueries, "default", _thisObject->getProperties());
	EntityPropertiesInteger::createProperty(m_groupMetadataFilter, m_propertyNumberOfQueriesMetadataSeries, 0, 0, m_maxNbOfQueriesMetadata, "default", _thisObject->getProperties());

	// Quantity Settings
	EntityPropertiesSelection* groupQuantityProp = EntityPropertiesSelection::createProperty(m_groupQuantitySettings, m_propertyName, { "" }, "", "default", _thisObject->getProperties());
	groupQuantityProp->setAllowCustomValues(true);
	//groupQuantityProp->setGroupChanges(true);

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
		//PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setGroupChanges(true);

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


std::list<std::string> PropertyBundleDataLakeQuery::getParameterOptions(const EntityBase* _thisObject) const
{
	const auto& options = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuerySettings + "_" + std::to_string(1))->getOptions();
	return { options.begin(), options.end() };
}

std::list<std::string> PropertyBundleDataLakeQuery::getQuantityOptions(const EntityBase* _thisObject) const
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
	bool projectChanged = PropertyHelper::getEntityProjectListProperty(_thisObject, m_propertyNameProjectName)->needsUpdate();
	auto seriesSelection = PropertyHelper::getSelectionProperty(_thisObject, m_propertyNameSeriesMetadata);
	bool seriesChanged = seriesSelection->needsUpdate();

	bool refreshNecessary = projectChanged;

	const std::list<MetadataSeries>& allSeries = _campaign.getSeriesMetadata();
	std::list<std::string> seriesNames;
	for (const MetadataSeries& series : allSeries)
	{
		seriesNames.push_back(series.getName());
	}
	seriesNames.push_front("");

	// reset all series metadata options
	if (projectChanged)
	{
		ot::JsonDocument overView;
		for (const MetadataSeries& series : allSeries)
		{
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
		setNameOptions(_thisObject, seriesNames, m_groupMetadataFilter, m_propertyNameSeriesMetadata);
	}

	// Now we filter the quantity and parameter options
	std::list<std::string> parameterLabel, quantityLabels;
	const auto& allParameterByLabel = _campaign.getMetadataParameterByLabel();
	auto allQuantityByLabel = _campaign.getMetadataQuantitiesByLabel();

	// Depending on the selected series, the quantity options will change.
	if (projectChanged || seriesChanged)
	{
		refreshNecessary = true;
		const std::string selectedSeries = seriesSelection->getValue();

		if (selectedSeries == "")
		{
			// reset to all possible quantities
			for (const auto& quantityByLabel : allQuantityByLabel)
			{
				quantityLabels.push_back(quantityByLabel.first);
			}
		}
		else
		{
			// We have a series filter set. 
			// First we check if the value is a valid series name
			if (std::find(seriesNames.begin(), seriesNames.end(), selectedSeries) == seriesNames.end())
			{
				// Its not a valid name, thus we check if it is a regex expression.
				RegexHelper::applyRegexFilter(seriesNames, selectedSeries);
				if (seriesNames.size() > 0)
				{
					for (const MetadataSeries& series : allSeries)
					{
						if (std::find(seriesNames.begin(), seriesNames.end(), series.getName()) != seriesNames.end())
						{
							for (auto& quantity : series.getQuantities())
							{
								quantityLabels.push_back(quantity.quantityLabel);
							}
						}
					}
					quantityLabels.sort();
					quantityLabels.unique();
				}
				else
				{
					// Here we set the options to emptry. 
				}
			}
			else
			{
				for (const MetadataSeries& series : allSeries)
				{
					if (series.getName() == selectedSeries)
					{
						for (auto& quantity : series.getQuantities())
						{
							quantityLabels.push_back(quantity.quantityLabel);
						}
					}
				}
			}
		}
		quantityLabels.push_front("");

		auto temp = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuantitySettings);
		std::list<std::string> currentOptions = { temp->getOptions().begin(), temp->getOptions().end() };
		if (currentOptions != quantityLabels)
		{
			temp->resetOptions(quantityLabels);
			temp->setNeedsUpdate();
		}
	}


	bool quantityChanged = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuantitySettings)->needsUpdate();
	const auto& availableQuantityLabels = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuantitySettings)->getOptions();
	std::list<std::string> availableQuantityLabelsList = { availableQuantityLabels.begin(), availableQuantityLabels.end() };
	if (projectChanged || seriesChanged || quantityChanged)
	{
		// Depending on the filtered quantity options, the parameter options need to be set to the depending parameters of the available quantity options.
		//If any of the above options have changed, we need to update the parameter options.
		const std::string currentQuantity = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyName, m_groupQuantitySettings);
		if (currentQuantity == "")
		{
			// If the quantity is empty we set the parameter options to all possible parameters.
			for (auto& parameterByLabel : allParameterByLabel)
			{
				parameterLabel.push_back(parameterByLabel.first);
			}
		}
		else if (allQuantityByLabel.find(currentQuantity) == allQuantityByLabel.end())
		//else if (std::find(avaliableQuantityLabels.begin(), avaliableQuantityLabels.end(), currentQuantity) == avaliableQuantityLabels.end())
		{
			// It may still be a regex expression. 
			RegexHelper::applyRegexFilter(availableQuantityLabelsList, currentQuantity);

			for (const std::string& quantityLabel : availableQuantityLabelsList)
			{
				if (quantityLabel != "")
				{
					const auto& dependentParameterLabel = allQuantityByLabel.find(quantityLabel)->second->dependingParameterLabels;
					parameterLabel.insert(parameterLabel.end(), dependentParameterLabel.begin(), dependentParameterLabel.end());
				}
			}
			parameterLabel.sort();
			parameterLabel.unique();
		}
		else
		{
			assert(allQuantityByLabel.find(currentQuantity) != allQuantityByLabel.end());
			const auto& dependentParameterLabel = allQuantityByLabel.find(currentQuantity)->second->dependingParameterLabels;
			parameterLabel.insert(parameterLabel.end(), dependentParameterLabel.begin(), dependentParameterLabel.end());
			
		}
		parameterLabel.push_front("");

		// If the parameter options have changed, we need to update them.
		const std::string groupName = m_groupQuerySettings + "_" + std::to_string(1);
		auto& temp = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->getOptions();
		std::list<std::string> currentOptions = { temp.begin(),temp.end() };
		if (currentOptions != parameterLabel)
		{
			refreshNecessary = true;
			for (uint32_t i = 1; i <= m_maxNbOfQueries; i++)
			{
				const std::string groupName = m_groupQuerySettings + "_" + std::to_string(i);
				setNameOptions(_thisObject, parameterLabel, groupName, m_propertyName);
			}
		}
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
