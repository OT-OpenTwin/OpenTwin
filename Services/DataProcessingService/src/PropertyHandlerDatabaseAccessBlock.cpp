#pragma once
#include "PropertyHandlerDatabaseAccessBlock.h"
#include "EntityPropertiesItems.h"
#include "EntityProperties.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "ClassFactory.h"
#include "OTCore/OwnerServiceGlobal.h"
#include "Application.h"
#include "ResultCollectionAccess.h"

void PropertyHandlerDatabaseAccessBlock::PerformUpdateIfRequired(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL)
{
	dbAccessEntity->getProperties();
	if (_bufferedInformation.find(dbAccessEntity->getEntityID()) != _bufferedInformation.end())
	{
		auto& buffer = _bufferedInformation[dbAccessEntity->getEntityID()];
		auto baseMSMDProperty = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameMeasurementSeries());
		auto msmdSelection = dynamic_cast<EntityPropertiesSelection*>(baseMSMDProperty);

		if (buffer.SelectedProject != dbAccessEntity->getSelectedProjectName())
		{
			EntityProperties campaignDependendProperties = UpdateAllCampaignDependencies(dbAccessEntity, sessionServiceURL, modelServiceURL);
			EntityProperties selectionDependendProperties = UpdateSelectionProperties(dbAccessEntity);
			campaignDependendProperties.merge(selectionDependendProperties);
			ot::UIDList entityIDs{ dbAccessEntity->getEntityID() };
			RequestPropertyUpdate(modelServiceURL, entityIDs, campaignDependendProperties.getJSON(nullptr, false));
		}
		else
		{
			std::string quantityValue, parameterValue1, parameterValue2, parameterValue3;
			getSelectedValues(dbAccessEntity, quantityValue, parameterValue1, parameterValue2, parameterValue3);
			if ( buffer.SelectedQuantity != quantityValue || buffer.SelectedParameter1 != parameterValue1 || buffer.SelectedParameter2 != parameterValue2 || buffer.SelectedParameter3 != parameterValue3)
			{
				EntityProperties selectionDependendProperties = UpdateSelectionProperties(dbAccessEntity);
				if (selectionDependendProperties.getNumberOfProperties() > 0)
				{
					ot::UIDList entityIDs{ dbAccessEntity->getEntityID() };
					RequestPropertyUpdate(modelServiceURL, entityIDs, selectionDependendProperties.getJSON(nullptr, false));
				}
			}
		}

		//if (buffer.SelectedParameter1 != parameter1Selection->getValue() || buffer.SelectedParameter2 != parameter2Selection->getValue() || buffer.SelectedParameter3 != parameter3Selection->getValue())
		//{
		//	return;
		//}
		//else if (buffer.SelectedMSMD != msmdSelection->getValue())
		//{
		//	//Update nothing yet (refresh Campaign metadata later on)
		//}
	}
	else
	{
		EntityProperties campaignDependendProperties = UpdateAllCampaignDependencies(dbAccessEntity, sessionServiceURL, modelServiceURL);
		EntityProperties selectionDependendProperties = UpdateSelectionProperties(dbAccessEntity);
		campaignDependendProperties.merge(selectionDependendProperties);
		ot::UIDList entityIDs{ dbAccessEntity->getEntityID() };
		RequestPropertyUpdate(modelServiceURL, entityIDs, campaignDependendProperties.getJSON(nullptr, false));
	}
}

EntityProperties PropertyHandlerDatabaseAccessBlock::UpdateAllCampaignDependencies(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL)
{
	const std::string projectName = dbAccessEntity->getSelectedProjectName();
	ResultCollectionAccess access(projectName, *_modelComponent, sessionServiceURL, modelServiceURL, &Application::instance()->getClassFactory());

	UpdateBuffer(dbAccessEntity, access.getMetadataCampaign());

	const std::string propertyNameMSMD = dbAccessEntity->getPropertyNameMeasurementSeries();
	
	BufferBlockDatabaseAccess& buffer = _bufferedInformation[dbAccessEntity->getEntityID()];
	EntityProperties properties;

	//EntityPropertiesSelection::createProperty(groupName1, propertyNameMSMD, msmdNames, buffer.SelectedMSMD, "default", properties);
	
	auto oldBaseQuantity = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameQuantity());
	auto oldSelectionQuantity = dynamic_cast<EntityPropertiesSelection*>(oldBaseQuantity);
	EntityPropertiesSelection* quantity = new EntityPropertiesSelection(*oldSelectionQuantity);
	quantity->resetOptions(buffer.QuantityNames);
	quantity->setValue(buffer.SelectedQuantity);
	properties.createProperty(quantity, dbAccessEntity->getGroupQuantity());

	auto oldBaseParameter1 = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameParameter1());
	auto oldSelectionParameter1 = dynamic_cast<EntityPropertiesSelection*>(oldBaseParameter1);
	EntityPropertiesSelection* parameter1 = new EntityPropertiesSelection(*oldSelectionParameter1);
	parameter1->resetOptions(buffer.ParameterNames);
	parameter1->setValue(buffer.SelectedParameter1);
	properties.createProperty(parameter1, dbAccessEntity->getGroupParameter1());

	auto oldBaseParameter2 = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameParameter2());
	auto oldSelectionParameter2 = dynamic_cast<EntityPropertiesSelection*>(oldBaseParameter2);
	EntityPropertiesSelection* parameter2 = new EntityPropertiesSelection(*oldSelectionParameter2);
	parameter2->resetOptions(buffer.ParameterNames);
	parameter2->setValue(buffer.SelectedParameter2);
	properties.createProperty(parameter2, dbAccessEntity->getGroupParameter2());
	
	auto oldBaseParameter3 = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameParameter3());
	auto oldSelectionParameter3 = dynamic_cast<EntityPropertiesSelection*>(oldBaseParameter3);
	EntityPropertiesSelection* parameter3 = new EntityPropertiesSelection(*oldSelectionParameter3);
	parameter3->resetOptions(buffer.ParameterNames);
	parameter3->setValue(buffer.SelectedParameter3);
	properties.createProperty(parameter3, dbAccessEntity->getGroupParameter3());

	return properties;
}

void PropertyHandlerDatabaseAccessBlock::RequestPropertyUpdate(const std::string& modelServiceURL, ot::UIDList entityIDs, const std::string& propertiesAsJSON)
{
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_UpdatePropertiesOfEntities,requestDoc.GetAllocator());
	ot::JsonObject jEntityIDs;
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDs,requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_JSON, ot::JsonString(propertiesAsJSON,requestDoc.GetAllocator()), requestDoc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", modelServiceURL, ot::EXECUTE, requestDoc.toJson(), response))
	{
		throw std::runtime_error("Updating properties failed due to error: " + response);
	}
}

void PropertyHandlerDatabaseAccessBlock::UpdateBuffer(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity,const MetadataCampaign& campaignMetadata)
{
	BufferBlockDatabaseAccess buffer;
	buffer.SelectedProject = dbAccessEntity->getSelectedProjectName();
	buffer.selectedDimension = dbAccessEntity->getQueryDimension();

	buffer.parameterByName = campaignMetadata.getMetadataParameterByName();
	buffer.quantitiesByName = campaignMetadata.getMetadataQuantitiesByName();
	std::list<MetadataSeries> seriesMetadata = campaignMetadata.getSeriesMetadata();

	std::string selectedQuantity, selectedParameter1, selectedParameter2, selectedParameter3;
	getSelectedValues(dbAccessEntity, selectedQuantity, selectedParameter1, selectedParameter2, selectedParameter3);

	//for (auto& msmd : seriesMetadata)
	//{
	//	msmdNames.push_back(msmd.getName());
	//}
	for (auto& quantity : buffer.quantitiesByName)
	{
		const std::string quantityName = quantity.first;
		if (quantityName == selectedQuantity)
		{
			buffer.SelectedQuantity = quantityName;
			buffer.dataTypeQuantity = quantity.second.typeName;
		}
		buffer.dataTypesByName[quantityName] = quantity.second.typeName;
		buffer.QuantityNames.push_back(quantityName);
	}
	for (auto parameter : buffer.parameterByName)
	{
		const std::string parameterName = parameter.first;
		if (parameterName == selectedParameter1)
		{
			buffer.SelectedParameter1 = parameterName;
			buffer.dataTypeParameter1 = parameter.second.typeName;
		}
		if (parameterName == selectedParameter2)
		{
			buffer.SelectedParameter2 = parameterName;
			buffer.dataTypeParameter2 = parameter.second.typeName;
		}
		if (parameterName == selectedParameter3)
		{
			buffer.SelectedParameter3 = parameterName;
			buffer.dataTypeParameter3 = parameter.second.typeName;
		}
		buffer.dataTypesByName[parameterName] = parameter.second.typeName;
		buffer.ParameterNames.push_back(parameterName);
	}

	_bufferedInformation[dbAccessEntity->getEntityID()] = buffer;
}

void PropertyHandlerDatabaseAccessBlock::getSelectedValues(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, std::string& outQuantityValue, std::string& outParameter1Value, std::string& outParameter2Value, std::string& outParameter3Value)
{
	auto baseQuantity= dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameQuantity());
	auto quantitySelection = dynamic_cast<EntityPropertiesSelection*>(baseQuantity);
	outQuantityValue = quantitySelection->getValue();

	auto baseParameter1 = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameParameter1());
	auto parameter1Selection = dynamic_cast<EntityPropertiesSelection*>(baseParameter1);
	outParameter1Value = parameter1Selection->getValue();

	auto baseParameter2 = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameParameter2());
	auto parameter2Selection = dynamic_cast<EntityPropertiesSelection*>(baseParameter2);
	outParameter2Value = parameter2Selection->getValue();

	auto baseParameter3 = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameParameter3());
	auto parameter3Selection = dynamic_cast<EntityPropertiesSelection*>(baseParameter3);
	outParameter3Value = parameter3Selection->getValue();
}

void PropertyHandlerDatabaseAccessBlock::getDataTypes(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, std::string& outQuantityType, std::string& outParameter1Type, std::string& outParameter2Type, std::string& outParameter3Type)
{
	auto baseQuantity = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyDataTypeQuantity());
	auto quantityType = dynamic_cast<EntityPropertiesString*>(baseQuantity);
	outQuantityType = quantityType->getValue();

	auto baseParameter1 = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyDataTypeParameter1());
	auto parameter1Type= dynamic_cast<EntityPropertiesString*>(baseParameter1);
	outParameter1Type = parameter1Type->getValue();

	auto baseParameter2 = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyDataTypeParameter2());
	auto parameter2Type= dynamic_cast<EntityPropertiesString*>(baseParameter2);
	outParameter2Type = parameter2Type->getValue();

	auto baseParameter3 = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyDataTypeParameter3());
	auto parameter3Type= dynamic_cast<EntityPropertiesString*>(baseParameter3);
	outParameter3Type = parameter3Type->getValue();
}

EntityProperties PropertyHandlerDatabaseAccessBlock::UpdateSelectionProperties(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity)
{
	auto& buffer = _bufferedInformation[dbAccessEntity->getEntityID()];

	std::string selectedQuantity, selectedParameter1, selectedParameter2, selectedParameter3;
	getSelectedValues(dbAccessEntity, selectedQuantity, selectedParameter1, selectedParameter2, selectedParameter3);
	buffer.SelectedQuantity = selectedQuantity;
	buffer.SelectedParameter1 = selectedParameter1;
	buffer.SelectedParameter2 = selectedParameter2;
	buffer.SelectedParameter3 = selectedParameter3;
	
	std::string quantityType, parameter1Type, parameter2Type, parameter3Type;
	getDataTypes(dbAccessEntity, quantityType, parameter1Type, parameter2Type, parameter3Type);

	EntityProperties properties;
	auto& oldProperties = dbAccessEntity->getProperties();
	if (quantityType != buffer.dataTypesByName[buffer.SelectedQuantity])
	{
		buffer.dataTypeQuantity = buffer.dataTypesByName[buffer.SelectedQuantity];
		CreateUpdatedTypeProperty(oldProperties.getProperty(dbAccessEntity->getPropertyDataTypeQuantity()), buffer.dataTypeQuantity, properties);
	}
	if (parameter1Type != buffer.dataTypesByName[buffer.SelectedParameter1])
	{
		buffer.dataTypeParameter1 = buffer.dataTypesByName[buffer.SelectedParameter1];
		CreateUpdatedTypeProperty(oldProperties.getProperty(dbAccessEntity->getPropertyDataTypeParameter1()) , buffer.dataTypeParameter1, properties);
	}
	if (parameter2Type != buffer.dataTypesByName[buffer.SelectedParameter2])
	{
		buffer.dataTypeParameter2 = buffer.dataTypesByName[buffer.SelectedParameter2];
		CreateUpdatedTypeProperty(oldProperties.getProperty(dbAccessEntity->getPropertyDataTypeParameter2()), buffer.dataTypeParameter2, properties);
	}
	if (parameter3Type != buffer.dataTypesByName[buffer.SelectedParameter3])
	{
		buffer.dataTypeParameter3 = buffer.dataTypesByName[buffer.SelectedParameter3];
		CreateUpdatedTypeProperty(oldProperties.getProperty(dbAccessEntity->getPropertyDataTypeParameter3()), buffer.dataTypeParameter3, properties);
	}

	return properties;
}

void PropertyHandlerDatabaseAccessBlock::CreateUpdatedTypeProperty(EntityPropertiesBase* oldEntity, const std::string& value, EntityProperties& properties)
{
	auto stringProp = dynamic_cast<EntityPropertiesString*>(oldEntity);
	EntityPropertiesString* newProperty = new EntityPropertiesString(*stringProp);
	newProperty->setValue(value);
	properties.createProperty(newProperty, newProperty->getGroup());
}