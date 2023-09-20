#pragma once
#include "PropertyHandlerDatabaseAccessBlock.h"
#include "EntityPropertiesItems.h"
#include "EntityProperties.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "CrossCollectionAccess.h"
#include "MeasurementCampaignFactory.h"
#include "OpenTwinCommunication/Msg.h"

void PropertyHandlerDatabaseAccessBlock::Buffer(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity)
{
	//if (_bufferedInformation.find(dbAccessEntity->getEntityID()) == _bufferedInformation.end())
	//{
	//	BufferBlockDatabaseAccess buffer;
	//	buffer.SelectedProject= dbAccessEntity->getSelectedProjectName();
	//	buffer.SelectedQueryDimension = dbAccessEntity->getQueryDimension();

	//	if (dbAccessEntity->getProperties().propertyExists(_quantityPropertyName))
	//	{
	//		auto baseQuantityProperty = dbAccessEntity->getProperties().getProperty(_quantityPropertyName);
	//		auto baseMSMDProperty = dbAccessEntity->getProperties().getProperty(_msmdPropertyName);
	//		auto quantitySelection = dynamic_cast<EntityPropertiesSelection*>(baseQuantityProperty);
	//		auto msmdSelection = dynamic_cast<EntityPropertiesSelection*>(baseMSMDProperty);

	//		buffer.QuantityNames = quantitySelection->getOptions();
	//		buffer.MeasurementMetadataNames = msmdSelection->getOptions();
	//		buffer.SelectedMSMD = msmdSelection->getValue();
	//	}
	//	else
	//	{
	//		buffer.QuantityNames = {};
	//		buffer.MeasurementMetadataNames = {};
	//		buffer.SelectedMSMD = "";
	//	}
	//	_bufferedInformation[dbAccessEntity->getEntityID()] = std::move(buffer);
	//}
	//else
	//{
	//	assert(0);
	//}
}

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
	const std::string propertyNameQuantity = "dbAccessEntity->getPropertyNameQuantity()";
	const std::string propertyNameParameter = "dbAccessEntity->getPropertyNameParameter()";

	const std::string groupName1 = "";
	BufferBlockDatabaseAccess& buffer = _bufferedInformation[dbAccessEntity->getEntityID()];
	EntityProperties properties;
	//EntityPropertiesSelection::createProperty(groupName1, propertyNameMSMD, msmdNames, buffer.SelectedMSMD, "default", properties);
	EntityPropertiesSelection::createProperty(groupName1, propertyNameQuantity, quantityNames, buffer.SelectedQuantity1, "default", properties);
	EntityPropertiesSelection::createProperty(groupName1, propertyNameParameter, parameterNames, buffer.SelectedParameter1, "default", properties);

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

void PropertyHandlerDatabaseAccessBlock::UpdateBuffer(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, std::list<std::string> msmdNames, std::list<std::string> parameterNames, std::list<std::string> quantityNames)
{
	BufferBlockDatabaseAccess buffer;
	buffer.SelectedProject = dbAccessEntity->getSelectedProjectName();
	buffer.SelectedQueryDimension =	dbAccessEntity->getQueryDimension();

	buffer.SelectedMSMD = "";
	buffer.SelectedParameter1 = "";
	buffer.SelectedQuantity1 = "";
	buffer.MeasurementMetadataNames.reserve(msmdNames.size());
	buffer.QuantityNames.reserve(quantityNames.size());
	buffer.ParameterNames.reserve(parameterNames.size());


	 //dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameMeasurementSeries());
	auto quantityPropertyBase = dbAccessEntity->getProperties().getProperty("");
	auto parameterPropertyBase = dbAccessEntity->getProperties().getProperty("");
	auto quantityPropertySelection = dynamic_cast<EntityPropertiesSelection*>(quantityPropertyBase);
	auto parameterPropertySelection = dynamic_cast<EntityPropertiesSelection*>(parameterPropertyBase);

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
		if (parameterName == parameterPropertySelection->getValue())
		{
			buffer.SelectedParameter1 = parameterName;
		}
		buffer.ParameterNames.push_back(parameterName);
	}
	for (std::string& quantityName : quantityNames)
	{
		if (quantityName == quantityPropertySelection->getValue())
		{
			buffer.SelectedQuantity1 = quantityName;
		}
		buffer.QuantityNames.push_back(quantityName);
	}

	_bufferedInformation[dbAccessEntity->getEntityID()] = buffer;
}

void PropertyHandlerDatabaseAccessBlock::PerformUpdateIfRequired(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL)
{
	if (_bufferedInformation.find(dbAccessEntity->getEntityID()) != _bufferedInformation.end())
	{
		auto oldBuffer = _bufferedInformation[dbAccessEntity->getEntityID()];
		auto baseMSMDProperty = dbAccessEntity->getProperties().getProperty(dbAccessEntity->getPropertyNameMeasurementSeries());
		auto msmdSelection = dynamic_cast<EntityPropertiesSelection*>(baseMSMDProperty);

		if (oldBuffer.SelectedProject != dbAccessEntity->getSelectedProjectName())
		{
			UpdateAllCampaignDependencies(dbAccessEntity,sessionServiceURL,modelServiceURL);
			//Update all
		}
		else if (oldBuffer.SelectedMSMD != msmdSelection->getValue())
		{
			//Update nothing yet (refresh Campaign metadata later on)
		}
		else if (oldBuffer.SelectedQueryDimension != dbAccessEntity->getQueryDimension())
		{
			//UpdateNumberQueryDimensions();
			//Update number of entries
		}
	}
	else
	{
		Buffer(dbAccessEntity);
		//UpdateAll();
		//Total update;
	}
}
