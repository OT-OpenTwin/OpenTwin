#pragma once
#include "PropertyHandlerDatabaseAccessBlock.h"
#include "EntityPropertiesItems.h"
#include "EntityProperties.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "ClassFactory.h"
#include "OTCore/OwnerServiceGlobal.h"
#include "Application.h"
#include "DataBase.h"

#include "ProjectToCollectionConverter.h"
#include "ResultCollectionMetadataAccess.h"
 
void PropertyHandlerDatabaseAccessBlock::performEntityUpdateIfRequired(std::shared_ptr<EntityBlockDatabaseAccess> _dbAccessEntity)
{
	std::string collectionName;
	std::unique_ptr<ResultCollectionMetadataAccess>resultCollectionAccess(getResultCollectionMetadataAccess(_dbAccessEntity.get(),collectionName));

	if (!resultCollectionAccess->collectionHasMetadata())
	{
		_uiComponent->displayMessage("Selected collection has no meta data and cannot be used by a database access block.\n");
	
	}
	else
	{
		EntityProperties newProperties;
		
		//First we check if the series selection must be updated
		std::list<std::string> allSeriesLabels  = resultCollectionAccess->listAllSeriesNames();
		//The user also needs the option to select no value
		allSeriesLabels.push_front(m_selectedValueNone);
		EntityPropertiesSelection* selectionSeries = _dbAccessEntity->getSeriesSelection();
		updateSelectionIfNecessary(allSeriesLabels, selectionSeries, newProperties);
		
		auto soFarAddedProperties =	newProperties.getListOfAllProperties();
		std::string selectedSeries;
		if (!soFarAddedProperties.empty())
		{
			//If a property was added at this point, it can only be a single property which is the series selection
			auto newSeriesSelection = dynamic_cast<EntityPropertiesSelection*>(soFarAddedProperties.back());
			newSeriesSelection->getValue();
		}
		else
		{
			//Otherwise take the selected series.
			selectedSeries = selectionSeries->getValue();
		}

		std::list<std::string> allQuantityLabels;
		if (selectedSeries == m_selectedValueNone)
		{
			allQuantityLabels = resultCollectionAccess->listAllQuantityLabels();
		}
		else
		{
			allQuantityLabels = resultCollectionAccess->listAllQuantityLabelsFromSeries(selectedSeries);
		}
		
		//Update Quantity Overview
		auto quantityValueCharacteristics = _dbAccessEntity->getQuantityValueCharacteristic();
		allQuantityLabels.push_front(m_selectedValueNone);
		updateSelectionIfNecessary(allQuantityLabels, quantityValueCharacteristics.m_label, newProperties);
		//Update Quantity labels and value description overview.
		std::list<std::string> dependingParameterLables = updateQuantityIfNecessary(_dbAccessEntity, resultCollectionAccess.get(),newProperties);

		//If the returned list is empty, no quantity has been selected and the list of parameter needs to be filled appropriately
		std::list<std::string> parameterList;
		if (dependingParameterLables.empty())
		{
			if (selectedSeries == "")
			{
				parameterList = resultCollectionAccess->listAllParameterLabels();
			}
			else
			{
				parameterList = resultCollectionAccess->listAllParameterLabelsFromSeries(selectedSeries);
			}
		}
		else
		{
			parameterList = dependingParameterLables;
		}
		parameterList.push_front(m_selectedValueNone);

		//Now we update all parameter overviews and the labels of the parameter
		auto parameterValueCharacteristics1 =_dbAccessEntity->getParameterValueCharacteristic1();
		updateSelectionIfNecessary(parameterList, parameterValueCharacteristics1.m_label, newProperties);
		updateParameterIfNecessary(*resultCollectionAccess, parameterValueCharacteristics1, newProperties);

		auto parameterValueCharacteristics2 = _dbAccessEntity->getParameterValueCharacteristic2();
		updateSelectionIfNecessary(parameterList, parameterValueCharacteristics2.m_label, newProperties);	
		updateParameterIfNecessary(*resultCollectionAccess, parameterValueCharacteristics2, newProperties);

		auto parameterValueCharacteristics3 = _dbAccessEntity->getParameterValueCharacteristic3();
		updateSelectionIfNecessary(parameterList, parameterValueCharacteristics3.m_label, newProperties);
		updateParameterIfNecessary(*resultCollectionAccess, parameterValueCharacteristics3, newProperties);

		if (!newProperties.getListOfAllProperties().empty())
		{
			ot::UIDList entityIDs{ _dbAccessEntity->getEntityID() };
			requestPropertyUpdate(entityIDs, newProperties.createJSON(nullptr,false));
		}
	}	
}

 ResultCollectionMetadataAccess* PropertyHandlerDatabaseAccessBlock::getResultCollectionMetadataAccess(EntityBlockDatabaseAccess* _dbAccessEntity, std::string& _collectionName)
{
	const std::string projectName = _dbAccessEntity->getSelectedProjectName();

	auto& classFactory = Application::instance()->getClassFactory();
	const std::string sessionServiceURL = Application::instance()->sessionServiceURL();
	auto modelComponent = Application::instance()->modelComponent();

	ProjectToCollectionConverter collectionFinder(sessionServiceURL);
	std::string loggedInUserName = Application::instance()->getLogInUserName();
	std::string loggedInUserPsw = Application::instance()->getLogInUserPsw();
	_collectionName = collectionFinder.NameCorrespondingCollection(projectName, loggedInUserName, loggedInUserPsw);

	const std::string thisProjectsName = DataBase::GetDataBase()->getProjectName();

	ResultCollectionMetadataAccess* newResultCollectionAccess;
	if (thisProjectsName == projectName)
	{
		newResultCollectionAccess =new ResultCollectionMetadataAccess(_collectionName, modelComponent, &classFactory);
	}
	else //Crosscollection access
	{
		newResultCollectionAccess = new ResultCollectionMetadataAccess(_collectionName, modelComponent, &classFactory, sessionServiceURL);
	}

	return newResultCollectionAccess;
}

void PropertyHandlerDatabaseAccessBlock::updateSelectionIfNecessary(std::list<std::string>& _valuesInProject, EntityPropertiesSelection* _selection, EntityProperties& _properties)
{
	const std::vector<std::string>& optionsSeries = _selection->getOptions();
	std::list<std::string> optionsSeriesList{ optionsSeries.begin(),optionsSeries.end() };
	if (optionsSeriesList != _valuesInProject)
	{
		EntityPropertiesSelection* newSelection = dynamic_cast<EntityPropertiesSelection*>(_selection->createCopy());
		newSelection->resetOptions(_valuesInProject);
		if (_valuesInProject.empty())
		{
			newSelection->setValue("");
		}
		else
		{
			const std::string selectedValue = newSelection->getValue();
			auto selectedValueInSelectedProject = std::find(_valuesInProject.begin(), _valuesInProject.end(), selectedValue);
			if (selectedValueInSelectedProject == _valuesInProject.end() || selectedValue == "")
			{
				newSelection->setValue(*_valuesInProject.begin());
			}
		}
		_properties.createProperty(newSelection, newSelection->getGroup());
	}
}

std::list<std::string> PropertyHandlerDatabaseAccessBlock::updateQuantityIfNecessary(std::shared_ptr<EntityBlockDatabaseAccess> _dbAccessEntity, ResultCollectionMetadataAccess* _resultCollectionAccess, EntityProperties& _properties)
{
	auto quantityValueCharacteristic = _dbAccessEntity->getQuantityValueCharacteristic();
	const std::string& selectedQuantityLabel = quantityValueCharacteristic.m_label->getValue();
	
	//If a quantity is selected we need to check for matching labels
	if (selectedQuantityLabel != m_selectedValueNone)
	{
		//If a quantity is selected, we need to check if it is still a valid one for the selected campaign.
		const MetadataQuantity* quantity = _resultCollectionAccess->findMetadataQuantity(selectedQuantityLabel);
		if (quantity == nullptr)
		{
			//Reset selected quantity label if necessary
			if (quantityValueCharacteristic.m_label->getValue() != m_selectedValueNone)
			{
				EntityPropertiesSelection* newLabelProperty = dynamic_cast<EntityPropertiesSelection*>(quantityValueCharacteristic.m_label->createCopy());
				assert(newLabelProperty != nullptr);
				newLabelProperty->setValue(m_selectedValueNone);
				_properties.createProperty(newLabelProperty, newLabelProperty->getGroup());
			}
			
			//Reset all labels, if necessary
			resetValueCharacteristicLabelsIfNecessary(quantityValueCharacteristic, _properties);

			//Reset value description selection
			EntityPropertiesSelection* selectionQuantityValDescr = _dbAccessEntity->getQuantityValueDescriptionSelection();
			std::list<std::string> emptyList{ };
			updateSelectionIfNecessary(emptyList, selectionQuantityValDescr, _properties);
			return std::list<std::string>();
		}

		//First we extract all parameter labels that shall be shown in relation to the selected quantity.
		const auto& dependingParameterIDs = quantity->dependingParameterIds;
		std::list<std::string> dependingParameterLables;
		for (ot::UID parameterID : dependingParameterIDs)
		{
			const MetadataParameter* parameter = _resultCollectionAccess->findMetadataParameter(parameterID);
			dependingParameterLables.push_back(parameter->parameterLabel);
		}

		//Now we check if the options of value descriptions is still up-to-date and the selected labels need to be updated
		EntityPropertiesSelection* selectionQuantityValDescr = _dbAccessEntity->getQuantityValueDescriptionSelection();
		const std::string& selectedQuantityValDescr = selectionQuantityValDescr->getValue();
		const auto& quantityValueDescriptions = quantity->valueDescriptions;
		std::string expectedUnit(""), expectedDataType("");
		std::list<std::string> valueDescriptionLabels;
		for (const auto& quantityValueDescription : quantityValueDescriptions)
		{
			if (quantityValueDescription.quantityValueLabel == selectedQuantityValDescr)
			{
				expectedUnit = quantityValueDescription.unit;
				expectedDataType = quantityValueDescription.dataTypeName;
			}
			valueDescriptionLabels.push_back(quantityValueDescription.quantityValueLabel);
		}
		updateSelectionIfNecessary(valueDescriptionLabels, selectionQuantityValDescr, _properties);
		
		
		const std::string selectedType = quantityValueCharacteristic.m_dataType->getValue();
		if (expectedDataType != selectedType)
		{
			EntityPropertiesString* newDataTypeProperty = dynamic_cast<EntityPropertiesString*>(quantityValueCharacteristic.m_dataType->createCopy());
			assert(newDataTypeProperty != nullptr);
			newDataTypeProperty->setValue(expectedDataType);
			_properties.createProperty(newDataTypeProperty, newDataTypeProperty->getGroup());
		}

		const std::string selectedUnit = quantityValueCharacteristic.m_unit->getValue();
		if (expectedUnit != selectedUnit)
		{
			EntityPropertiesString* newUnitProperty = dynamic_cast<EntityPropertiesString*>(quantityValueCharacteristic.m_unit->createCopy());
			assert(newUnitProperty != nullptr);
			newUnitProperty->setValue(expectedUnit);
			_properties.createProperty(newUnitProperty, newUnitProperty->getGroup());
		}
		return dependingParameterLables;
	}
	else
	{
		//If no quantity is selected, we need to check if the type and unit labels need to be reset.
		resetValueCharacteristicLabelsIfNecessary(quantityValueCharacteristic, _properties);
		EntityPropertiesSelection* selectionQuantityValDescr = _dbAccessEntity->getQuantityValueDescriptionSelection();
		std::list<std::string> emptyList { };
		updateSelectionIfNecessary(emptyList, selectionQuantityValDescr, _properties);
		return std::list<std::string>();
	}
}

void PropertyHandlerDatabaseAccessBlock::updateParameterIfNecessary(const ResultCollectionMetadataAccess& _resultAccess, const ValueCharacteristicProperties& _selectedProperties, EntityProperties& _properties)
{
	const std::string& label = _selectedProperties.m_label->getValue();
	if (label != m_selectedValueNone)
	{
		const MetadataParameter* parameter = _resultAccess.findMetadataParameter(label);
		if (parameter == nullptr)
		{
			if (_selectedProperties.m_label->getValue() != m_selectedValueNone)
			{
				//Reset selected parameter label
				EntityPropertiesSelection* newParameterLabel = dynamic_cast<EntityPropertiesSelection*>(_selectedProperties.m_label->createCopy());
				newParameterLabel->setValue(m_selectedValueNone);
				_properties.createProperty(newParameterLabel, newParameterLabel->getGroup());
			}
			resetValueCharacteristicLabelsIfNecessary(_selectedProperties, _properties);
		}
		else
		{
			const std::string& expectedType = parameter->typeName;
			const std::string & selectedType = _selectedProperties.m_dataType->getValue();
			//Update data type property if necessary
			if (expectedType != selectedType)
			{
				EntityPropertiesString* newDataTypeProperty = dynamic_cast<EntityPropertiesString*>(_selectedProperties.m_dataType->createCopy());
				assert(newDataTypeProperty != nullptr);
				newDataTypeProperty->setValue(expectedType);
				_properties.createProperty(newDataTypeProperty, newDataTypeProperty->getGroup());
			}

			//Update unit property if necessary
			const std::string& expectedUnit = parameter->unit;
			const std::string & selectedUnit = _selectedProperties.m_unit->getValue();
			if(expectedUnit != selectedUnit)	
			{
				EntityPropertiesString* newDataTypeProperty = dynamic_cast<EntityPropertiesString*>(_selectedProperties.m_unit->createCopy());
				assert(newDataTypeProperty != nullptr);
				newDataTypeProperty->setValue(expectedUnit);
				_properties.createProperty(newDataTypeProperty, newDataTypeProperty->getGroup());
			}
		}
	}
	else
	{
		//If no parameter is set,  we need to check if the type and unit labels need to be reset.
		resetValueCharacteristicLabelsIfNecessary(_selectedProperties, _properties);
	}
}

void PropertyHandlerDatabaseAccessBlock::resetValueCharacteristicLabelsIfNecessary(const ValueCharacteristicProperties& _selectedProperties, EntityProperties& _properties)
{
	const std::string selectedType = _selectedProperties.m_dataType->getValue();
	if (selectedType != m_selectedValueNone)
	{
		resetValueCharacteristicLabelDataType(_selectedProperties, _properties);
	}

	const std::string selectedUnit = _selectedProperties.m_unit->getValue();
	if (selectedUnit != m_selectedValueNone)
	{
		resetValueCharacteristicLabelUnit(_selectedProperties, _properties);
	}
}

void PropertyHandlerDatabaseAccessBlock::resetValueCharacteristicLabelUnit(const ValueCharacteristicProperties& _selectedProperties, EntityProperties& _properties)
{
	EntityPropertiesString* newUnitProperty = dynamic_cast<EntityPropertiesString*>(_selectedProperties.m_unit->createCopy());
	assert(newUnitProperty != nullptr);
	newUnitProperty->setValue(m_selectedValueNone);
	_properties.createProperty(newUnitProperty, newUnitProperty->getGroup());
}

void PropertyHandlerDatabaseAccessBlock::resetValueCharacteristicLabelDataType(const ValueCharacteristicProperties& _selectedProperties, EntityProperties& _properties)
{
	EntityPropertiesString* newDataTypeProperty = dynamic_cast<EntityPropertiesString*>(_selectedProperties.m_dataType->createCopy());
	assert(newDataTypeProperty != nullptr);
	newDataTypeProperty->setValue(m_selectedValueNone);
	_properties.createProperty(newDataTypeProperty, newDataTypeProperty->getGroup());
}

void PropertyHandlerDatabaseAccessBlock::requestPropertyUpdate(ot::UIDList entityIDs, const std::string& propertiesAsJSON)
{
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_UpdatePropertiesOfEntities,requestDoc.GetAllocator());
	ot::JsonObject jEntityIDs;
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDs,requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_JSON, ot::JsonString(propertiesAsJSON, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	
	const std::string& modelServiceURL = Application::instance()->modelComponent()->getServiceURL();

	std::string response;
	if (!ot::msg::send("", modelServiceURL, ot::EXECUTE, requestDoc.toJson(), response))
	{
		throw std::runtime_error("Updating properties failed due to error: " + response);
	}
}