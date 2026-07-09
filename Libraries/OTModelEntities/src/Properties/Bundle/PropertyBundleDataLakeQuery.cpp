// @otlicense

// OpenTwin header
#include "OTCore/ComparisonSymbols.h"
#include "OTCore/DataFilter/RegexHelper.h"
#include "OTCore/Tuple/TupleFactory.h"
#include "OTModelEntities/Properties/PropertyHelper.h"
#include "OTModelEntities/Properties/Bundle/PropertyBundleDataLakeQuery.h"

// std header
#include <set>
#include <unordered_set>
#include "OTCore/DataStruct/GenericDataStructMatrix.h"
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
	EntityPropertiesInteger::createProperty(m_groupMetadataFilter, m_propertyNumberOfQueries, 0, 0, m_maxNbOfQueries, "default", _thisObject->getProperties())->setAllowCustomValues(false);
	EntityPropertiesInteger::createProperty(m_groupMetadataFilter, m_propertyNumberOfQueriesMetadataSeries, 0, 0, m_maxNbOfQueriesMetadata, "default", _thisObject->getProperties())->setAllowCustomValues(false);

	// Quantity Settings
	EntityPropertiesSelection* groupQuantityProp = EntityPropertiesSelection::createProperty(m_groupQuantitySettings, m_propertyName, { "" }, "", "default", _thisObject->getProperties());
	groupQuantityProp->setAllowCustomValues(true);
		// Tuple properties
	EntityPropertiesSelection* quantityComponentProp = EntityPropertiesSelection::createProperty(m_groupQuantitySettings, m_propertyQuantityComponent, { "" }, "", "default", _thisObject->getProperties());
	quantityComponentProp->setVisible(false);
	EntityPropertiesSelection* tupleFormat = EntityPropertiesSelection::createProperty(m_groupQuantitySettings, m_propertyTupleFormat, { "" }, "", "default", _thisObject->getProperties());
	tupleFormat->setVisible(false);
	EntityPropertiesSelection* tupleUnits = EntityPropertiesSelection::createProperty(m_groupQuantitySettings, m_propertyTupleUnit, { "" }, "", "default", _thisObject->getProperties());
	tupleUnits->setVisible(false);
		//Matrix properties
	auto row = EntityPropertiesInteger::createProperty(m_groupMatrixOptions, m_propertyMatrixRow, 1,1, 500 , "default", _thisObject->getProperties());
	row->setVisible(false);
	auto column = EntityPropertiesInteger::createProperty(m_groupMatrixOptions, m_propertyMatrixColumn, 1,1,500, "default", _thisObject->getProperties());
	column->setVisible(false);

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
	
	uint32_t numberVisible = static_cast<uint32_t>(numberOfMetadataQueries->getValue());
	for (uint32_t i = 1; i <= m_maxNbOfQueriesMetadata; i++)
	{
		bool shallBeVisible = (i <= numberVisible);
		const std::string groupName = m_groupSeriesMetadata + "_" + std::to_string(i);
		auto valueProp = PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName);

		if (valueProp->getVisible() != shallBeVisible)
		{
			valueProp->setVisible(shallBeVisible);
			PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(shallBeVisible);
			PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(shallBeVisible);
			requiresUpdate |= true;
		}
	}
	
	return requiresUpdate;
}


std::list<std::string> PropertyBundleDataLakeQuery::getParameterOptions(const EntityBase* _thisObject) const
{
	const auto options = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuerySettings + "_" + std::to_string(1))->getOptionStrings();
	return { options.begin(), options.end() };
}

std::list<std::string> PropertyBundleDataLakeQuery::getQuantityOptions(const EntityBase* _thisObject) const
{
	const auto options = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuantitySettings)->getOptionStrings();
	return { options.begin(), options.end() };
}

std::list<std::string> PropertyBundleDataLakeQuery::getTupleOptions(const EntityBase* _thisObject) const
{
	const EntityPropertiesSelection* selection = dynamic_cast<const EntityPropertiesSelection*>(PropertyHelper::getSelectionProperty(_thisObject, m_propertyQuantityComponent, m_groupQuantitySettings));
	const auto options = selection->getOptionStrings();
	if (options.size() > 0)
	{
		std::list<std::string> optionsWithoutTupleType = { std::next(options.begin()), options.end()};
		return optionsWithoutTupleType;
	}
	return {};
}

std::string PropertyBundleDataLakeQuery::getTupleType(const EntityBase* _thisObject) const
{
	const EntityPropertiesSelection* selection = dynamic_cast<const EntityPropertiesSelection*>(PropertyHelper::getSelectionProperty(_thisObject, m_propertyQuantityComponent, m_groupQuantitySettings));
	const auto& options = selection->getOptions();
	if (options.size() > 0)
	{
		return options.front().first;
	}
	return {};
}

std::string PropertyBundleDataLakeQuery::getSelectedProject(EntityBase* _thisObject)
{
	return PropertyHelper::getProjectPropertyValue(_thisObject, m_propertyNameProjectName);
}
std::string PropertyBundleDataLakeQuery::getSelectedSeries(EntityBase* _thisObject)
{
	return PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyNameSeriesMetadata, m_groupMetadataFilter);
}

std::pair<uint32_t, std::string>  PropertyBundleDataLakeQuery::getMatrixIndex(const EntityBase* _thisObject) const
{
	auto columnProp = PropertyHelper::getIntegerProperty(_thisObject, m_propertyMatrixColumn, m_groupMatrixOptions);
	if (columnProp->getVisible())
	{
		auto rowProp = PropertyHelper::getIntegerProperty(_thisObject, m_propertyMatrixRow, m_groupMatrixOptions);

		uint32_t selectedColumn = static_cast<uint32_t>(columnProp->getValue());
		uint32_t selectedRow = static_cast<uint32_t>(rowProp->getValue());
		std::string indexLabel = std::to_string(selectedRow) + std::to_string(selectedColumn);
		//Switch from 1-based indexing (UI) to 0-based indexing (internal)
		ot::MatrixEntryPointer selectedPoint(selectedRow-1, selectedColumn-1);

		ot::MatrixEntryPointer dimensions;
		dimensions.setRow(static_cast<uint32_t>(rowProp->getMax()));
		dimensions.setColumn(static_cast<uint32_t>(columnProp->getMax()));

		ot::GenericDataStructMatrix matrix(dimensions);
		uint32_t index = matrix.getIndex(selectedPoint);
		return { index, indexLabel };
	}
	else
	{
		return { -1,"" };
	}
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
		try
		{
			for (const MetadataSeries& series : allSeries)
			{
				const ot::JsonDocument& metadata = series.getMetadata();
				ot::json::mergeObjects(overView, metadata, overView.GetAllocator());
			}
		}
		catch (const std::exception& _e)
		{
			OT_USER_LOG_E("Failed to merge metadata overview: " + std::string(_e.what()));
			overView = ot::JsonDocument();
		}
		
		std::list<std::string> allOptions;
		vectorize(overView, allOptions, "");
		allOptions.push_front("");
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
		const auto currentOptions = temp->getOptionStrings();
		std::list<std::string> currentOptionsList = { currentOptions.begin(), currentOptions.end() };
		if (currentOptionsList != quantityLabels)
		{
			temp->resetOptions(quantityLabels);
			temp->setNeedsUpdate();
		}
	}


	bool quantityChanged = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuantitySettings)->needsUpdate();
	const auto availableQuantityLabels = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuantitySettings)->getOptionStrings();

	std::list<std::string> availableQuantityLabelsList = { availableQuantityLabels.begin(), availableQuantityLabels.end() };
	if (projectChanged || seriesChanged || quantityChanged)
	{
		// Depending on the filtered quantity options, the parameter options need to be set to the depending parameters of the available quantity options.
		//If any of the above options have changed, we need to update the parameter options.
		
		//We need to check if the selected quantities matching the entered value are tuples. If so, and they are all of the same type, we need the option to target a specific component of the tuple.
		std::set<std::string> tupleTypes;
		std::unordered_set<ot::MatrixEntryPointer> matrixDimensions;
		const std::string currentQuantity = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyName, m_groupQuantitySettings);
		if (currentQuantity == "")
		{
			// If the quantity is empty we set the parameter options to all possible parameters.
			for (auto& parameterByLabel : allParameterByLabel)
			{
				parameterLabel.push_back(parameterByLabel.first);
			}
			
			// The component selection shall only be visible if the selected quantity is a tuple.
			refreshNecessary |= setTuplePropertyVisibility(_thisObject,false);
		}
		else if (allQuantityByLabel.find(currentQuantity) == allQuantityByLabel.end())
		{
			// It may still be a regex expression. 
			RegexHelper::applyRegexFilter(availableQuantityLabelsList, currentQuantity);
			for (const std::string& quantityLabel : availableQuantityLabelsList)
			{
				if (quantityLabel != "")
				{
					auto matchingQuantityByLabel = allQuantityByLabel.find(quantityLabel);
					assert(matchingQuantityByLabel != allQuantityByLabel.end()); 
					const auto& dependentParameterLabel = matchingQuantityByLabel->second->dependingParameterLabels;
					parameterLabel.insert(parameterLabel.end(), dependentParameterLabel.begin(), dependentParameterLabel.end());
					ot::TupleInstance& quantityTupleDesc = matchingQuantityByLabel->second->m_tupleDescription;
					if (!quantityTupleDesc.isSingle())
					{
						tupleTypes.insert(quantityTupleDesc.getTupleTypeName());
					}
					
					const std::vector<uint32_t>& dimensions =	matchingQuantityByLabel->second->dataDimensions;
					if (dimensions.size() == 2)
					{
						matrixDimensions.insert(ot::MatrixEntryPointer(dimensions[0], dimensions[1]));
					}
					
				}
			}
			parameterLabel.sort();
			parameterLabel.unique();
		}
		else
		{
			assert(allQuantityByLabel.find(currentQuantity) != allQuantityByLabel.end());
			const MetadataQuantity* matchingQuantity =	allQuantityByLabel.find(currentQuantity)->second;
			const auto& dependentParameterLabel = matchingQuantity->dependingParameterLabels;
			parameterLabel.insert(parameterLabel.end(), dependentParameterLabel.begin(), dependentParameterLabel.end());

			const ot::TupleInstance& quantityTuple = matchingQuantity->m_tupleDescription;
			if (!quantityTuple.isSingle())
			{
				tupleTypes.insert(quantityTuple.getTupleTypeName());
			}
			
			const std::vector<uint32_t>& dimensions = matchingQuantity->dataDimensions;
			if (dimensions.size() == 2)
			{
				matrixDimensions.insert(ot::MatrixEntryPointer(dimensions[0], dimensions[1]));
			}
		}
		parameterLabel.push_front("");

		// If the parameter options have changed, we need to update them.
		const std::string groupName = m_groupQuerySettings + "_" + std::to_string(1);
		const auto temp = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->getOptionStrings();
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

		// Here we set the tuple options, depending on the selected quantity. If valid
		// First we process the tuple settings of the matched quantities
		if (tupleTypes.size() == 0)
		{
			// None of the matching quantities was a tuple, thus we hide the associated properties.
			refreshNecessary |= setTuplePropertyVisibility(_thisObject, false);
		}
		else if (tupleTypes.size() == 1)
		{
			// Valid case. We have a single tuple type among the matched quantities. Now we show the tuple properties and update the selection options accordingly
			refreshNecessary |= setTuplePropertyVisibility(_thisObject, true);

			const std::string tupleType = *tupleTypes.begin();
			ot::TupleDescription* tupleDescription = TupleFactory::create(tupleType);


			assert(tupleDescription != nullptr);
			const std::vector<std::string>& formatNames = tupleDescription->getAllTupleFormatNames();
			std::string selectedFormat =  PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyTupleFormat, m_groupQuantitySettings);
			if (selectedFormat == "")
			{
				// Happens if the options are filled the first time
				selectedFormat = formatNames.front();
			}
			std::vector<std::string> targetElements = tupleDescription->getTupleElementNames(selectedFormat);
			targetElements.insert(targetElements.begin(), tupleType); // This is for performing a query on the entire tuple
			std::vector<std::string> unitOptions =tupleDescription->getUnitCombinations(selectedFormat);
			refreshNecessary |=	setTupleSelectionOptions(_thisObject, formatNames, targetElements, unitOptions);

			size_t numberOfQuantityOptions = PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuantitySettings)->getOptions().size() -1 ;// -1 since the empty option "" was already added.
			if (numberOfQuantityOptions != 1)
			{
				OT_LOG_W("Selected quantities are not exclusively tuples. Inconsistent tuple properties of the quantity cannot be queried.");
			}
		}
		else
		{
			// Invalid case. We cannot show multiple tuple types at the same time. We would need to specify the query characteristics for each tuple type separately, which is currently not supported by the property design.
			refreshNecessary |= setTuplePropertyVisibility(_thisObject, false);
			OT_LOG_W("Selected quantities are different types of tuple and cannot be queried simultaneously.");
		}

		// Here we set the matrix options, depending on the selected quantity.
		if (matrixDimensions.size() == 0)
		{
			refreshNecessary |= setMatrixPropertyVisibility(_thisObject, false);
		}
		else if (matrixDimensions.size() == 1)
		{
			refreshNecessary |= setMatrixPropertyVisibility(_thisObject, true);
			setMatrixDimension(_thisObject, *matrixDimensions.begin());
		}
		else
		{
			refreshNecessary |= setTuplePropertyVisibility(_thisObject, false);
			OT_LOG_W("Selected quantities have different matrix dimensions and cannot be queried simultaneously.");
		}

	}

	// Much of the tuple update logic is already handled when the quantity has changed. However, a format change also requires some updates
	if (!quantityChanged)
	{
		EntityPropertiesSelection* formatSelection = dynamic_cast<EntityPropertiesSelection*>(PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleFormat, m_groupQuantitySettings));
		if (formatSelection->needsUpdate())
		{
			EntityPropertiesSelection* componentSelection = dynamic_cast<EntityPropertiesSelection*>(PropertyHelper::getSelectionProperty(_thisObject, m_propertyQuantityComponent, m_groupQuantitySettings));
			std::string tupleName;
			const auto& options = componentSelection->getOptions();
			if (!options.empty())
			{
				tupleName = options.front().first;
			}

			ot::TupleDescription* tupleDescription = TupleFactory::create(tupleName);
			if (tupleDescription != nullptr)
			{
				auto tupleFormats = tupleDescription->getAllTupleFormatNames();
				const std::string newFormat = formatSelection->getValue();
				auto componentNames =	tupleDescription->getTupleElementNames(newFormat);
				componentNames.insert(componentNames.begin(), tupleName);
				auto units = tupleDescription->getUnitCombinations(newFormat);
				setTupleSelectionOptions(_thisObject, tupleFormats, componentNames, units);	
				PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleFormat, m_groupQuantitySettings)->setValue(newFormat);
			}
			else
			{
				// When setting the component options, the first entry should be the tuple type name. This serves as a way to target the entire tuple, e.g. query for a complex number.
				assert(false); 
			}
		}

		// A change in matrix index also needs an update
		refreshNecessary |= PropertyHelper::getIntegerProperty(_thisObject, m_propertyMatrixRow, m_groupMatrixOptions)->needsUpdate();
		refreshNecessary |= PropertyHelper::getIntegerProperty(_thisObject, m_propertyMatrixColumn, m_groupMatrixOptions)->needsUpdate();
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


	for (uint32_t i = 1; i <= m_maxNbOfQueriesMetadata; i++)
	{
		const std::string groupName = m_groupSeriesMetadata + "_" + std::to_string(i);
		refreshNecessary |= PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->needsUpdate();
		refreshNecessary |= PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->needsUpdate();
		refreshNecessary |= PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->needsUpdate();		
	}

	refreshNecessary |= PropertyHelper::getIntegerProperty(_thisObject, m_propertyNumberOfQueriesMetadataSeries, m_groupMetadataFilter)->needsUpdate();
	
	return refreshNecessary;
}

bool PropertyBundleDataLakeQuery::tuplePropertiesAreVisible(const EntityBase* _thisObject) const
{
	return PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleFormat, m_groupQuantitySettings)->getVisible();
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
	const std::string value = PropertyHelper::getStringPropertyValue(_thisObject, m_propertyValue, m_groupQuantitySettings);
	const std::string name = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyName, m_groupQuantitySettings);
	const std::string comparator = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyComparator, m_groupQuantitySettings);
	
	ot::TupleInstance instance;
	ot::ValueComparisonDescription valueComparisonDefinition(name, comparator, value,instance);
	if (PropertyHelper::getSelectionProperty(_thisObject, m_propertyQuantityComponent, m_groupQuantitySettings)->getVisible())
	{
		//In this case we have a tuple
		const std::string tupleTargetElement = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyQuantityComponent, m_groupQuantitySettings);
		valueComparisonDefinition.setTupleTarget(tupleTargetElement);
		
		const std::string tupleTargetUnit = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyTupleUnit, m_groupQuantitySettings);
		std::vector<std::string> units = ot::TupleDescription::separateCombinedUnitString(tupleTargetUnit);
		instance.setTupleUnits(units);

		EntityPropertiesSelection* componentSelection = dynamic_cast<EntityPropertiesSelection*>(PropertyHelper::getSelectionProperty(_thisObject, m_propertyQuantityComponent, m_groupQuantitySettings));
		std::string tupleName;
		const auto& options = componentSelection->getOptions();
		if (!options.empty())
		{
			tupleName = options.front().first;
		}
		instance.setTupleTypeName(tupleName);

		const std::string tupleTargetFormat = PropertyHelper::getSelectionPropertyValue(_thisObject, m_propertyTupleFormat, m_groupQuantitySettings);
		instance.setTupleFormatName(tupleTargetFormat);

		instance.setTupleElementDataTypes({ ot::TypeNames::getDoubleTypeName(),ot::TypeNames::getDoubleTypeName() });
		
		valueComparisonDefinition.setTupleInstance(instance);
	}
	else
	{
		const std::string dataType = PropertyHelper::getStringPropertyValue(_thisObject, m_propertyDataType, m_groupQuantitySettings);
		const std::string unit = PropertyHelper::getStringPropertyValue(_thisObject, m_propertyUnit, m_groupQuantitySettings);
		instance.setTupleElementDataTypes({ dataType });
		instance.setTupleUnits({ unit });
		valueComparisonDefinition.setTupleInstance(instance);



	}
	uint32_t index = getMatrixIndex(_thisObject).first;
	valueComparisonDefinition.setDimensionIndex(index);
	
	return valueComparisonDefinition;
}

std::list<ot::ValueComparisonDescription> PropertyBundleDataLakeQuery::getParameterQueries(EntityBase* _thisObject) const
{
	std::list<ot::ValueComparisonDescription> valueComparisonDefinitions;
	
	int32_t considerableQueries = PropertyHelper::getIntegerPropertyValue(_thisObject, m_propertyNumberOfQueries, m_groupMetadataFilter);
		
	for (int32_t i = 1; i <= considerableQueries; i++)
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
	int32_t considerableQueries = PropertyHelper::getIntegerPropertyValue(_thisObject, m_propertyNumberOfQueriesMetadataSeries, m_groupMetadataFilter);
	std::list<ot::ValueComparisonDescription> definitions;
	for (int32_t index = 1; index <= considerableQueries; index++)
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

bool PropertyBundleDataLakeQuery::setTuplePropertyVisibility(EntityBase* _thisObject, bool _visible)
{
	bool requiresRefreshing;
	EntityPropertiesBase* selection = PropertyHelper::getSelectionProperty(_thisObject, m_propertyQuantityComponent, m_groupQuantitySettings);
	requiresRefreshing = selection->getVisible() != _visible;
	selection->setVisible(_visible);
	PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleFormat, m_groupQuantitySettings)->setVisible(_visible);
	PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleUnit, m_groupQuantitySettings)->setVisible(_visible);

	// The tuple unit property is a selection prop, while the regular unit prop is a string prop, so we switch between them.
	PropertyHelper::getStringProperty(_thisObject, m_propertyUnit, m_groupQuantitySettings)->setVisible(!_visible);
	
	return requiresRefreshing;
}

bool PropertyBundleDataLakeQuery::setMatrixPropertyVisibility(EntityBase* _thisObject, bool _visible)
{
	bool requiresRefreshing;
	EntityPropertiesBase* rowProperty = PropertyHelper::getIntegerProperty(_thisObject, m_propertyMatrixRow, m_groupMatrixOptions);
	requiresRefreshing = rowProperty->getVisible() != _visible;
	rowProperty->setVisible(_visible);
	PropertyHelper::getIntegerProperty(_thisObject, m_propertyMatrixColumn, m_groupMatrixOptions)->setVisible(_visible);
	return requiresRefreshing;
}

void PropertyBundleDataLakeQuery::setMatrixDimension(EntityBase* _thisObject, const ot::MatrixEntryPointer& _matrixEntryPointer)
{
	PropertyHelper::getIntegerProperty(_thisObject, m_propertyMatrixRow, m_groupMatrixOptions)->setMax(static_cast<int32_t>(_matrixEntryPointer.getRow()));
	PropertyHelper::getIntegerProperty(_thisObject, m_propertyMatrixColumn, m_groupMatrixOptions)->setMax(static_cast<int32_t>(_matrixEntryPointer.getColumn()));
}

ot::MatrixEntryPointer PropertyBundleDataLakeQuery::getMatrixDimension(EntityBase* _thisObject) const
{
	ot::MatrixEntryPointer pointer;
	pointer.setRow(static_cast<uint32_t>(PropertyHelper::getIntegerPropertyValue(_thisObject, m_propertyMatrixRow, m_groupMatrixOptions)));
	pointer.setColumn(static_cast<uint32_t>(PropertyHelper::getIntegerPropertyValue(_thisObject, m_propertyMatrixColumn, m_groupMatrixOptions)));
	return pointer;
}

bool PropertyBundleDataLakeQuery::setTupleSelectionOptions(EntityBase* _thisObject, const std::vector<std::string>& _formatOptions, const std::vector<std::string>& _elementOptions, const std::vector<std::string>& _unitOptions)
{
	bool refresh = false;
	EntityPropertiesSelection* selectionComponent = dynamic_cast<EntityPropertiesSelection*>(PropertyHelper::getSelectionProperty(_thisObject, m_propertyQuantityComponent, m_groupQuantitySettings));
	refresh |= selectionComponent->getOptionStrings() != _elementOptions;
	selectionComponent->resetOptions(_elementOptions);
	selectionComponent->setNeedsUpdate();
	
	EntityPropertiesSelection* selectionFormat =dynamic_cast<EntityPropertiesSelection*>(PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleFormat, m_groupQuantitySettings));
	refresh |= selectionFormat->getOptionStrings() != _formatOptions;
	selectionFormat->resetOptions(_formatOptions);
	selectionFormat->setNeedsUpdate();

	EntityPropertiesSelection* selectionUnits = dynamic_cast<EntityPropertiesSelection*>(PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleUnit, m_groupQuantitySettings));
	refresh |= selectionUnits->getOptionStrings() != _unitOptions;
	selectionUnits->resetOptions(_unitOptions);
	selectionUnits->setNeedsUpdate();
	return refresh;
}


void PropertyBundleDataLakeQuery::reset(EntityBase* _thisObject, const DataLakeQueryCfg& _config)
{
	for (uint32_t i = 1; i <= m_maxNbOfQueries; i++)
	{
		const std::string groupName = m_groupQuerySettings + "_" + std::to_string(i);
		PropertyHelper::setStringPropertyValue("", _thisObject, m_propertyDataType, groupName);
		PropertyHelper::setStringPropertyValue("", _thisObject, m_propertyUnit, groupName);
		PropertyHelper::setStringPropertyValue("", _thisObject, m_propertyValue, groupName);

		std::list<std::string> parameterOptions = { _config.getParameterOptions().begin(), _config.getParameterOptions().end() };
		parameterOptions.push_front("");
		PropertyHelper::setSelectionPropertyValue(parameterOptions, _thisObject, m_propertyName, groupName);
		PropertyHelper::setSelectionPropertyValue({ _config.getParameterOptions().begin(), _config.getParameterOptions().end() }, _thisObject, m_propertyName, groupName);
		PropertyHelper::setSelectionPropertyValue(std::list<std::string>{ "" }, _thisObject, m_propertyComparator, groupName);

		PropertyHelper::getStringProperty(_thisObject, m_propertyDataType, groupName)->setVisible(false);
		PropertyHelper::getStringProperty(_thisObject, m_propertyUnit, groupName)->setVisible(false);
		PropertyHelper::getStringProperty( _thisObject, m_propertyValue, groupName)->setVisible(false);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(false);
		PropertyHelper::getSelectionProperty( _thisObject, m_propertyComparator, groupName)->setVisible(false);
	}
	const std::list<ot::ValueComparisonDescription>& paramComp = _config.getValueDescriptionParameters();
	assert(static_cast<uint32_t>(paramComp.size()) <= m_maxNbOfQueries);
	
	PropertyHelper::setIntegerPropertyValue(static_cast<int32_t>(paramComp.size()), _thisObject, m_propertyNumberOfQueries, m_groupMetadataFilter);
	auto currentParamComp = paramComp.begin();
	for (size_t i = 1; i <= paramComp.size(); i++)
	{
		const std::string groupName = m_groupQuerySettings + "_" + std::to_string(i);
		
		const ot::TupleInstance& tuple = currentParamComp->getTupleInstance();
		assert(tuple.isSingle()); // parameter are not allowed to be tuple.
		
		PropertyHelper::setStringPropertyValue(tuple.getTupleElementDataTypes().front(), _thisObject, m_propertyDataType, groupName);
		PropertyHelper::setStringPropertyValue(tuple.getTupleUnits().front(), _thisObject, m_propertyUnit, groupName);
		PropertyHelper::setStringPropertyValue(currentParamComp->getValue(), _thisObject, m_propertyValue, groupName);
		PropertyHelper::setSelectionPropertyValue(currentParamComp->getName(), _thisObject, m_propertyName, groupName);
		PropertyHelper::setSelectionPropertyValue(std::list<std::string>{ currentParamComp->getComparator() }, _thisObject, m_propertyComparator, groupName);

		PropertyHelper::getStringProperty(_thisObject, m_propertyDataType, groupName)->setVisible(true);
		PropertyHelper::getStringProperty(_thisObject, m_propertyUnit, groupName)->setVisible(true);
		PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setVisible(true);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(true);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(true);

		if (currentParamComp != paramComp.end())
		{
			currentParamComp++;
		}
	}

	ot::ValueComparisonDescription quantityComparison = _config.getValueDescriptionQuantities();
	const ot::TupleInstance& quantityTuple = 	quantityComparison.getTupleInstance();
	if (!quantityTuple.isSingle())
	{
		// Component, format, units, data type, value, comp, name
		std::list<std::string>quantityComponents = { quantityTuple.getTupleTypeName() };
		if (quantityComparison.getTupleTarget() != "")
		{
			quantityComponents.push_back(quantityComparison.getTupleTarget());
		}
		PropertyHelper::setSelectionPropertyValue(quantityComponents, _thisObject, m_propertyQuantityComponent, m_groupQuantitySettings);
		if (quantityComparison.getTupleTarget() != "")
		{
			PropertyHelper::setSelectionPropertyValue(quantityComparison.getTupleTarget(), _thisObject, m_propertyQuantityComponent, m_groupQuantitySettings);
		}
		const std::string combinedUnit = ot::TupleDescription::createCombinedUnitsString(quantityTuple.getTupleUnits());
		PropertyHelper::setSelectionPropertyValue(std::list<std::string>{ combinedUnit }, _thisObject, m_propertyTupleUnit, m_groupQuantitySettings);
		PropertyHelper::setSelectionPropertyValue(std::list<std::string>{ quantityTuple.getTupleFormatName() }, _thisObject, m_propertyTupleFormat, m_groupQuantitySettings);


		PropertyHelper::getSelectionProperty(_thisObject, m_propertyQuantityComponent, m_groupQuantitySettings)->setVisible(true);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleUnit, m_groupQuantitySettings)->setVisible(true);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleFormat, m_groupQuantitySettings)->setVisible(true);
		PropertyHelper::getStringProperty( _thisObject, m_propertyUnit, m_groupQuantitySettings)->setVisible(false);
	}
	else
	{
		PropertyHelper::setStringPropertyValue(quantityTuple.getTupleUnits().front(), _thisObject, m_propertyUnit, m_groupQuantitySettings);
		
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyQuantityComponent, m_groupQuantitySettings)->setVisible(false);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleUnit, m_groupQuantitySettings)->setVisible(false);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleFormat, m_groupQuantitySettings)->setVisible(false);
		PropertyHelper::getStringProperty(_thisObject, m_propertyUnit, m_groupQuantitySettings)->setVisible(true);
	}
	PropertyHelper::setStringPropertyValue(quantityTuple.getTupleTypeName(), _thisObject, m_propertyDataType, m_groupQuantitySettings);
	PropertyHelper::setSelectionPropertyValue(std::list<std::string>{ quantityComparison.getName() }, _thisObject, m_propertyName, m_groupQuantitySettings);
	PropertyHelper::setStringPropertyValue(quantityComparison.getValue(), _thisObject, m_propertyValue, m_groupQuantitySettings);
	PropertyHelper::setSelectionPropertyValue(std::list<std::string>{quantityComparison.getComparator()}, _thisObject, m_propertyComparator, m_groupQuantitySettings);
	
	PropertyHelper::setProjectPropertyValue(_config.getCollectionName(), _thisObject, m_propertyNameProjectName, m_groupMetadataFilter);
	PropertyHelper::setSelectionPropertyValue(std::list<std::string>{ _config.getSeriesLabel() }, _thisObject, m_propertyNameSeriesMetadata, m_groupMetadataFilter);

	for (uint32_t i = 1; i <= m_maxNbOfQueriesMetadata; i++)
	{
		const std::string groupName = m_groupSeriesMetadata + "_" + std::to_string(i);
		
		PropertyHelper::setSelectionPropertyValue(std::list<std::string>{ "" }, _thisObject, m_propertyName, groupName);
		PropertyHelper::setStringPropertyValue("",_thisObject, m_propertyValue, groupName);
		PropertyHelper::setSelectionPropertyValue(std::list<std::string>{ "" }, _thisObject, m_propertyComparator, groupName);

		PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(false);
		PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setVisible(false);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(false);
	}

	std::list<ot::ValueComparisonDescription> metadataComp = _config.getValueDescriptionSeriesMD();
	PropertyHelper::setIntegerPropertyValue(static_cast<int32_t>(metadataComp.size()), _thisObject, m_propertyNumberOfQueriesMetadataSeries, m_groupMetadataFilter);
	auto currentMetadataComp = metadataComp.begin();
	for (size_t i = 1; i <= metadataComp.size(); i++)
	{
		const std::string groupName = m_groupSeriesMetadata + "_" + std::to_string(i);

		PropertyHelper::setSelectionPropertyValue(std::list<std::string>{ currentMetadataComp->getName() }, _thisObject, m_propertyName, groupName);
		PropertyHelper::setStringPropertyValue(currentMetadataComp->getValue(), _thisObject, m_propertyValue, groupName);
		PropertyHelper::setSelectionPropertyValue(std::list<std::string>{ currentMetadataComp->getComparator() }, _thisObject, m_propertyComparator, groupName);

		PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setVisible(true);
		PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setVisible(true);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setVisible(true);

		if (currentMetadataComp != metadataComp.end())
		{
			currentMetadataComp++;
		}
	}

	const std::vector<uint32_t>& dimensions = _config.getMatrixDimensions();
	if (dimensions.size() == 2)
	{
		ot::MatrixEntryPointer selectedPoint(dimensions[0], dimensions[1]);
		setMatrixDimension(_thisObject, selectedPoint);
		setMatrixPropertyVisibility(_thisObject, true);
	}
}


void PropertyBundleDataLakeQuery::lockAllProperties(EntityBase* _thisObject)
{
	PropertyHelper::getSelectionProperty(_thisObject, m_propertyQuantityComponent, m_groupQuantitySettings)->setReadOnly(true);
	PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleUnit, m_groupQuantitySettings)->setReadOnly(true);
	PropertyHelper::getSelectionProperty(_thisObject, m_propertyTupleFormat, m_groupQuantitySettings)->setReadOnly(true);
	PropertyHelper::getStringProperty(_thisObject, m_propertyUnit, m_groupQuantitySettings)->setReadOnly(true);
	PropertyHelper::getStringProperty(_thisObject, m_propertyDataType, m_groupQuantitySettings)->setReadOnly(true);
	PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, m_groupQuantitySettings)->setReadOnly(true);
	PropertyHelper::getStringProperty(_thisObject, m_propertyValue, m_groupQuantitySettings)->setReadOnly(true);
	PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, m_groupQuantitySettings)->setReadOnly(true);

	PropertyHelper::getEntityProjectListProperty(_thisObject, m_propertyNameProjectName, m_groupMetadataFilter)->setReadOnly(true);
	PropertyHelper::getSelectionProperty(_thisObject, m_propertyNameSeriesMetadata, m_groupMetadataFilter)->setReadOnly(true);
	PropertyHelper::getIntegerProperty(_thisObject, m_propertyNumberOfQueriesMetadataSeries, m_groupMetadataFilter)->setReadOnly(true);
	PropertyHelper::getIntegerProperty(_thisObject, m_propertyNumberOfQueries, m_groupMetadataFilter)->setReadOnly(true);

	for (uint32_t i = 1; i <= m_maxNbOfQueriesMetadata; i++)
	{
		const std::string groupName = m_groupSeriesMetadata + "_" + std::to_string(i);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setReadOnly(true);
		PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setReadOnly(true);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setReadOnly(true);
	}

	for (uint32_t i = 1; i <= m_maxNbOfQueries; i++)
	{
		const std::string groupName = m_groupQuerySettings + "_" + std::to_string(i);
		
		PropertyHelper::getStringProperty(_thisObject, m_propertyDataType, groupName)->setReadOnly(true);
		PropertyHelper::getStringProperty(_thisObject, m_propertyUnit, groupName)->setReadOnly(true);
		PropertyHelper::getStringProperty(_thisObject, m_propertyValue, groupName)->setReadOnly(true);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyName, groupName)->setReadOnly(true);
		PropertyHelper::getSelectionProperty(_thisObject, m_propertyComparator, groupName)->setReadOnly(true);
	}
}


bool PropertyBundleDataLakeQuery::getOrderReproducible(const EntityBase* _thisObject) const
{
	return PropertyHelper::getBoolPropertyValue(_thisObject, m_propertyOrder, m_groupQuerySettings);
}
