#pragma once
#include "PropertyHandlerDatabaseAccessBlock.h"
#include "EntityPropertiesItems.h"
#include "EntityProperties.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "CrossCollectionAccess.h"
#include "MeasurementCampaignFactory.h"
#include "OpenTwinCommunication/Msg.h"

void PropertyHandlerDatabaseAccessBlock::PerformUpdateIfRequired(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL)
{
	if (_bufferedInformation.find(dbAccessEntity->getEntityID()) != _bufferedInformation.end())
	{
		auto buffer = _bufferedInformation[dbAccessEntity->getEntityID()];
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
	const MeasurementCampaign measurementCampaign =	GetMeasurementCampaign(dbAccessEntity, sessionServiceURL, modelServiceURL);

	UpdateBuffer(dbAccessEntity, measurementCampaign);

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

const MeasurementCampaign PropertyHandlerDatabaseAccessBlock::GetMeasurementCampaign(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL)
{
	CrossCollectionAccess access(dbAccessEntity->getSelectedProjectName(), sessionServiceURL, modelServiceURL);
	if (access.ConnectedWithCollection())
	{
		auto rmd = access.getMeasurementCampaignMetadata(_modelComponent);
		auto msmds = access.getMeasurementMetadata(_modelComponent);

		MeasurementCampaignFactory factory;
		const MeasurementCampaign measurementCampaign = factory.Create(rmd, msmds);
		return measurementCampaign;
	}
	else
	{
		throw std::exception("Could not connect with collection");
	}
}

void PropertyHandlerDatabaseAccessBlock::RequestPropertyUpdate(const std::string& modelServiceURL, ot::UIDList entityIDs, const std::string& propertiesAsJSON)
{
	OT_rJSON_createDOC(requestDoc);
	ot::rJSON::add(requestDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_UpdatePropertiesOfEntities);
	ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityIDList, entityIDs);
	ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_JSON, propertiesAsJSON);
	
	std::string response;
	if (!ot::msg::send("", modelServiceURL, ot::EXECUTE, ot::rJSON::toJSON(requestDoc), response))
	{
		throw std::runtime_error("Updating properties failed due to error: " + response);
	}
}

void PropertyHandlerDatabaseAccessBlock::UpdateBuffer(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const MeasurementCampaign& campaignMetadata)
{
	BufferBlockDatabaseAccess buffer;
	buffer.SelectedProject = dbAccessEntity->getSelectedProjectName();
	
	std::map <std::string, MetadataParameter> parameters = campaignMetadata.getMetadataParameter();
	std::map <std::string, MetadataQuantity> quantities = campaignMetadata.getMetadataQuantities();
	std::list<SeriesMetadata> seriesMetadata = campaignMetadata.getSeriesMetadata();

	std::string selectedQuantity, selectedParameter1, selectedParameter2, selectedParameter3;
	getSelectedValues(dbAccessEntity, selectedQuantity, selectedParameter1, selectedParameter2, selectedParameter3);

	//for (auto& msmd : seriesMetadata)
	//{
	//	msmdNames.push_back(msmd.getName());
	//}
	for (auto& quantity : quantities)
	{
		const std::string quantityName = quantity.first;
		if (quantityName == selectedQuantity)
		{
			buffer.SelectedQuantity = quantityName;
			buffer.dataTypeQuantity = quantity.second.typeName;
		}
		buffer.QuantityNames.push_back(quantityName);
	}
	for (auto parameter : parameters)
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

void PropertyHandlerDatabaseAccessBlock::getDataTypes(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, std::string& outQuantityType, std::string& outParameter1Type, std::string& outParameter2Type, std::string& outParameter3Type, bool& outParameter2Visible, bool& outParameter3Visible)
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
	outParameter2Visible = parameter2Type->getVisible();

	auto baseParameter3 = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyDataTypeParameter3());
	auto parameter3Type= dynamic_cast<EntityPropertiesString*>(baseParameter3);
	outParameter3Type = parameter3Type->getValue();
	outParameter3Visible = parameter3Type->getVisible();

}

EntityProperties PropertyHandlerDatabaseAccessBlock::UpdateSelectionProperties(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity)
{
	auto& buffer = _bufferedInformation[dbAccessEntity->getEntityID()];

	std::string quantityType, parameter1Type, parameter2Type, parameter3Type;
	bool parameter2Visible, parameter3Visible;
	getDataTypes(dbAccessEntity, quantityType, parameter1Type, parameter2Type, parameter3Type, parameter2Visible, parameter3Visible);

	auto& oldProperties = dbAccessEntity->getProperties();
	EntityProperties properties;
	if (quantityType != buffer.dataTypeQuantity)
	{
		
		CreateUpdatedTypeProperty(oldProperties.getProperty(dbAccessEntity->getPropertyDataTypeQuantity()), buffer.dataTypeQuantity, properties);
	}
	
	if (parameter1Type != buffer.dataTypeParameter1)
	{
		CreateUpdatedTypeProperty(oldProperties.getProperty(dbAccessEntity->getPropertyDataTypeParameter1()) , buffer.dataTypeParameter1, properties);
	}
	
	if (parameter2Type != buffer.dataTypeParameter2)
	{
		CreateUpdatedTypeProperty(oldProperties.getProperty(dbAccessEntity->getPropertyDataTypeParameter2()), buffer.dataTypeParameter2, properties);
	}
	
	if (parameter3Type != buffer.dataTypeParameter3)
	{
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


