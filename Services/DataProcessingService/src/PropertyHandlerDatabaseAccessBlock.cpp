#pragma once
#include "PropertyHandlerDatabaseAccessBlock.h"
#include "EntityPropertiesItems.h"
#include "EntityProperties.h"
#include "OpenTwinCommunication/ActionTypes.h"

void PropertyHandlerDatabaseAccessBlock::Buffer(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity)
{
	if (_bufferedInformation.find(dbAccessEntity->getEntityID()) == _bufferedInformation.end())
	{
		BufferBlockDatabaseAccess buffer;
		buffer.SelectedProject= dbAccessEntity->getSelectedProjectName();
		
		if (dbAccessEntity->getProperties().propertyExists(_quantityPropertyName))
		{
			auto baseQuantityProperty = dbAccessEntity->getProperties().getProperty(_quantityPropertyName);
			auto baseMSMDProperty = dbAccessEntity->getProperties().getProperty(_msmdPropertyName);
			auto quantitySelection = dynamic_cast<EntityPropertiesSelection*>(baseQuantityProperty);
			auto msmdSelection = dynamic_cast<EntityPropertiesSelection*>(baseMSMDProperty);

			buffer.QuantityNames = quantitySelection->getOptions();
			buffer.MeasurementMetadataNames = msmdSelection->getOptions();
			buffer.SelectedMSMD = msmdSelection->getValue();
		}
		else
		{
			buffer.QuantityNames = {};
			buffer.MeasurementMetadataNames = {};
			buffer.SelectedMSMD = "";
		}
		_bufferedInformation[dbAccessEntity->getEntityID()] = std::move(buffer);
	}
	else
	{
		assert(0);
	}
}

bool PropertyHandlerDatabaseAccessBlock::requiresUpdate(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity)
{
	if (_bufferedInformation.find(dbAccessEntity->getEntityID()) != _bufferedInformation.end())
	{
		BufferBlockDatabaseAccess& buffer = _bufferedInformation[dbAccessEntity->getEntityID()];
		if (buffer.SelectedProject != dbAccessEntity->getSelectedProjectName())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		Buffer(dbAccessEntity);
		return true;
	}
}

OT_rJSON_doc PropertyHandlerDatabaseAccessBlock::Update(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity, std::list<std::string>& quantityNames, std::list<std::string>& parameterNames, std::list<std::string>& msmdNames)
{
	
	EntityProperties properties;
	auto baseMSMDProperty = dbAccessEntity->getProperties().getProperty(_msmdPropertyName);
	auto msmdSelection = dynamic_cast<EntityPropertiesSelection*>(baseMSMDProperty);
	BufferBlockDatabaseAccess buffer;
	buffer.MeasurementMetadataNames.reserve(msmdNames.size());
	buffer.SelectedProject = dbAccessEntity->getSelectedProjectName();

	std::string defaultValue = "";
	for (std::string msmdName : msmdNames)
	{
		if (msmdName == msmdSelection->getValue())
		{
			defaultValue = msmdSelection->getValue();
		}
		buffer.MeasurementMetadataNames.push_back(msmdName);
	}
	EntityPropertiesSelection::createProperty(_groupQuerySpecifications, _msmdPropertyName, msmdNames, defaultValue, "default", properties);


	auto baseQuantityProperty = dbAccessEntity->getProperties().getProperty(_quantityPropertyName);
	auto quantitySelection = dynamic_cast<EntityPropertiesSelection*>(baseQuantityProperty);
	buffer.QuantityNames.reserve(quantityNames.size());

	defaultValue = "";
	for (std::string quantityName : quantityNames)
	{
		if (quantityName == quantitySelection->getValue())
		{
			defaultValue = quantitySelection->getValue();
		}
		buffer.QuantityNames.push_back(quantityName);
	}
	EntityPropertiesSelection::createProperty(_groupQuerySpecifications, _quantityPropertyName, quantityNames,defaultValue, "default", properties);

	OT_rJSON_createDOC(requestDoc);
	ot::rJSON::add(requestDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_UpdatePropertiesOfEntities);
	ot::UIDList ids{ dbAccessEntity->getEntityID() };
	ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityIDList, ids);
	ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_JSON, properties.getJSON(nullptr, true));
	

	_bufferedInformation[dbAccessEntity->getEntityID()] = buffer;

	return requestDoc;
}
