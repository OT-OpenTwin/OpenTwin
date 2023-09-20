#pragma once
#include "PropertyHandlerDatabaseAccessBlock.h"
#include "EntityPropertiesItems.h"
#include "EntityProperties.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "CrossCollectionAccess.h"
#include "MeasurementCampaignFactory.h"
#include "OpenTwinCommunication/Msg.h"

void PropertyHandlerDatabaseAccessBlock::UpdateAllCampaignDependencies(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL)
{
	const MeasurementCampaign measurementCampaign =	GetMeasurementCampaign(dbAccessEntity, sessionServiceURL, modelServiceURL);
	std::list<std::string> msmdNames, parameterNames, quantityNames;
	
	std::map <std::string, MetadataParameter> parameters = measurementCampaign.getMetadataParameter();
	std::map <std::string, MetadataQuantity> quantities = measurementCampaign.getMetadataQuantities();
	std::list<SeriesMetadata> seriesMetadata = measurementCampaign.getSeriesMetadata();

	for (auto& quantity : quantities)
	{
		quantityNames.push_back(quantity.first);
	}
	for (auto& msmd : seriesMetadata)
	{
		msmdNames.push_back(msmd.getName());
	}
	for (auto parameter : parameters)
	{
		parameterNames.push_back(parameter.first);
	}

	UpdateBuffer(dbAccessEntity, msmdNames, quantityNames, parameterNames);

	const std::string propertyNameMSMD = dbAccessEntity->getPropertyNameMeasurementSeries();
	
	BufferBlockDatabaseAccess& buffer = _bufferedInformation[dbAccessEntity->getEntityID()];
	EntityProperties properties;

	//EntityPropertiesSelection::createProperty(groupName1, propertyNameMSMD, msmdNames, buffer.SelectedMSMD, "default", properties);
	EntityPropertiesSelection::createProperty(dbAccessEntity->getGroupQuantity(), dbAccessEntity->getPropertyNameQuantity(), quantityNames, buffer.SelectedQuantity, "default", properties);
	EntityPropertiesSelection::createProperty(dbAccessEntity->getGroupParameter1(), dbAccessEntity->getPropertyNameParameter1(), parameterNames, buffer.SelectedParameter1, "default", properties);
	EntityPropertiesSelection::createProperty(dbAccessEntity->getGroupParameter2(), dbAccessEntity->getPropertyNameParameter2(), parameterNames, buffer.SelectedParameter2, "default", properties);
	EntityPropertiesSelection::createProperty(dbAccessEntity->getGroupParameter3(), dbAccessEntity->getPropertyNameParameter3(), parameterNames, buffer.SelectedParameter3, "default", properties);

	const ot::UIDList entityIDs{ dbAccessEntity->getEntityID() };
	RequestPropertyUpdate(modelServiceURL, entityIDs, properties.getJSON(nullptr,false));
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

void PropertyHandlerDatabaseAccessBlock::UpdateBuffer(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, std::list<std::string>& msmdNames, std::list<std::string>& parameterNames, std::list<std::string>& quantityNames)
{
	BufferBlockDatabaseAccess buffer;
	buffer.SelectedProject = dbAccessEntity->getSelectedProjectName();
	
	buffer.SelectedMSMD = "";
	buffer.SelectedQuantity = "";
	buffer.SelectedParameter1 = "";
	buffer.SelectedParameter2 = "";
	buffer.SelectedParameter3 = "";
	buffer.MeasurementMetadataNames.reserve(msmdNames.size());
	buffer.QuantityNames.reserve(quantityNames.size());
	buffer.ParameterNames.reserve(parameterNames.size());


	//dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameMeasurementSeries());
	std::string selectedQuantity, selectedParameter1, selectedParameter2, selectedParameter3;
	getSelectedValues(dbAccessEntity,selectedQuantity,selectedParameter1, selectedParameter2, selectedParameter3);

	/*for (std::string msmdName : msmdNames)
	{
		if (msmdName == msmdSelection->getValue())
		{
			defaultValue = msmdSelection->getValue();
		}
		buffer.MeasurementMetadataNames.push_back(msmdName);
	}*/

	for (std::string& parameterName : parameterNames)
	{
		if (parameterName == selectedParameter1)
		{
			buffer.SelectedParameter1 = parameterName;
		}
		if (parameterName == selectedParameter2)
		{
			buffer.SelectedParameter2 = parameterName;
		}
		if (parameterName == selectedParameter3)
		{
			buffer.SelectedParameter3 = parameterName;
		}
		buffer.ParameterNames.push_back(parameterName);
	}
	for (std::string& quantityName : quantityNames)
	{
		if (quantityName == selectedQuantity)
		{
			buffer.SelectedQuantity = quantityName;
		}
		buffer.QuantityNames.push_back(quantityName);
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

void PropertyHandlerDatabaseAccessBlock::PerformUpdateIfRequired(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL)
{
	if (_bufferedInformation.find(dbAccessEntity->getEntityID()) != _bufferedInformation.end())
	{
		auto buffer = _bufferedInformation[dbAccessEntity->getEntityID()];
		auto baseMSMDProperty = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameMeasurementSeries());
		auto msmdSelection = dynamic_cast<EntityPropertiesSelection*>(baseMSMDProperty);


		if (buffer.SelectedProject != dbAccessEntity->getSelectedProjectName())
		{
			UpdateAllCampaignDependencies(dbAccessEntity,sessionServiceURL,modelServiceURL);
			//Update all
			return;
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
		UpdateAllCampaignDependencies(dbAccessEntity, sessionServiceURL, modelServiceURL);
		
		//Total update;
	}
}
