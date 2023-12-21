#include "MetadataEntityInterface.h"
#include "MetadataEntry.h"
#include "MetadataEntryArray.h"
#include "MetadataEntryObject.h"
#include "MetadataEntrySingle.h"

#include <vector>

MetadataCampaign MetadataEntityInterface::CreateCampaign(std::shared_ptr<EntityMetadataCampaign> rmd, std::list<std::shared_ptr<EntityMetadataSeries>> msmds)
{
	MetadataCampaign measurementCampaign;
	ExtractCampaignMetadata(measurementCampaign, rmd);
	ExtractSeriesMetadata(measurementCampaign, msmds);
	measurementCampaign.UpdateMetadataOverview();
	return measurementCampaign;
}

void MetadataEntityInterface::StoreCampaign(ot::components::ModelComponent& modelComponent, MetadataCampaign& metaDataCampaign)
{
	EntityMetadataCampaign entityCampaign(modelComponent.createEntityUID(), nullptr, nullptr, nullptr, nullptr, "");
	for (auto& metadata : metaDataCampaign.getMetaData())
	{
		//ToDo
	}
	entityCampaign.StoreToDataBase();
	_newEntityIDs.push_back(entityCampaign.getEntityID());
	_newEntityVersions.push_back(entityCampaign.getEntityStorageVersion());
}

void MetadataEntityInterface::StoreCampaign(ot::components::ModelComponent& modelComponent, MetadataCampaign& metaDataCampaign, std::list<const MetadataSeries*>& seriesMetadata)
{
	StoreCampaign(modelComponent, metaDataCampaign);
	StoreCampaign(modelComponent, seriesMetadata);
}

void MetadataEntityInterface::StoreCampaign(ot::components::ModelComponent& modelComponent, std::list<const MetadataSeries*>& seriesMetadata)
{
	std::list< EntityMetadataSeries> entitiesMetadataSeries;
	for (auto& newSeriesMetadata : seriesMetadata)
	{
		const std::string name = newSeriesMetadata->getName();
		EntityMetadataSeries entitySeries(modelComponent.createEntityUID(), nullptr, nullptr, nullptr, nullptr, "");
		entitySeries.setName(name);
		for (const MetadataParameter& parameter : newSeriesMetadata->getParameter())
		{
			MetadataParameter& parameterForChange = const_cast<MetadataParameter&>(parameter);
			entitySeries.InsertToParameterField(_nameField, { ot::Variable(parameter.parameterName) }, parameter.parameterAbbreviation);
			entitySeries.InsertToParameterField(_valuesField, std::move(parameterForChange.values), parameter.parameterAbbreviation);
			for (auto& metadata : parameter.metaData)
			{
				//ToDo
			}
		}
		for (const MetadataQuantity& quantity : newSeriesMetadata->getQuantities())
		{
			entitySeries.InsertToQuantityField(_nameField, { ot::Variable(quantity.quantityName) }, quantity.quantityAbbreviation);
			entitySeries.InsertToQuantityField(_datatypeField, { ot::Variable(quantity.typeName) }, quantity.quantityAbbreviation);
			for (auto& metadata : quantity.metaData)
			{
				//ToDo
			}
		}
		for (auto& metadata : newSeriesMetadata->getMetadata())
		{
			//ToDo
		}
		entitySeries.StoreToDataBase();
		_newEntityIDs.push_back(entitySeries.getEntityID());
		_newEntityVersions.push_back(entitySeries.getEntityStorageVersion());
	}

	std::list<bool> visibillity(false);
	modelComponent.addEntitiesToModel(std::move(_newEntityIDs), std::move(_newEntityVersions), std::move(visibillity), {}, {}, {}, "Updated result data collection");
}

void MetadataEntityInterface::ExtractCampaignMetadata(MetadataCampaign& measurementCampaign, std::shared_ptr<EntityMetadataCampaign> rmd)
{
	const GenericDocument* topLevel= rmd->getDocumentTopLevel();
	auto fieldList = ExtractMetadataFields(*topLevel);
	measurementCampaign.setCampaignName(rmd->getName());
	for (auto& field : fieldList)
	{
		measurementCampaign.AddMetaInformation(field->getEntryName(), field);
	}
	auto objectList = ExtractMetadataObjects(*topLevel);
	for (auto& object : objectList)
	{
		measurementCampaign.AddMetaInformation(object->getEntryName(), object);
	}
}

std::list<std::shared_ptr<MetadataEntry>> MetadataEntityInterface::ExtractMetadataObjects(const GenericDocument& document)
{
	auto& subDocuments = document.getSubDocuments();
	std::list<std::shared_ptr<MetadataEntry>> allMetadata;
	for (const GenericDocument* subDoc : subDocuments)
	{
		std::shared_ptr<MetadataEntryObject> object(new MetadataEntryObject(subDoc->getDocumentName()));
		auto fieldList = ExtractMetadataFields(*subDoc);
		object->AddMetadataEntry(fieldList);
		auto subObjectList = ExtractMetadataObjects(*subDoc);
		object->AddMetadataEntry(subObjectList);
		allMetadata.push_back(object);
	}
	return allMetadata;
}

std::list<std::shared_ptr<MetadataEntry>> MetadataEntityInterface::ExtractMetadataFields(const GenericDocument& document)
{
	auto fields = document.getFields();
	std::list<std::shared_ptr<MetadataEntry>> metadata;
	for (auto field = fields->begin(); field != fields->end(); field++)
	{
		const std::string name = field->first;
		const std::list<ot::Variable>& values = field->second;
		if (values.size() == 1)
		{
			metadata.push_back(std::shared_ptr<MetadataEntry>(new MetadataEntrySingle(name, *values.begin())));
		} 
		else
		{
			metadata.push_back(std::shared_ptr<MetadataEntry>(new MetadataEntryArray(name, values)));
		}
	}
	return metadata;
}

void MetadataEntityInterface::ExtractSeriesMetadata(MetadataCampaign& measurementCampaign, std::list<std::shared_ptr<EntityMetadataSeries>> msmds)
{
	for (auto msmd : msmds)
	{
		std::string entityName = msmd->getName();
		const std::string name = entityName.substr(entityName.find_last_of("/") +1);
		MetadataSeries seriesMetadata (name);
		
		const GenericDocument* parameterTopLevel = msmd->getDocument(msmd->getParameterDocumentName());
		const std::vector<const GenericDocument*> allParameterDocuments = parameterTopLevel->getSubDocuments();
		for (const GenericDocument* parameterDocument : allParameterDocuments)
		{
			MetadataParameter parameter;
			parameter.parameterAbbreviation = parameterDocument->getDocumentName();
			auto parameterFields = ExtractMetadataFields(*parameterDocument);
			for (std::shared_ptr<MetadataEntry> entry : parameterFields)
			{
				if (entry->getEntryName() == _nameField)
				{
					auto nameEntry = dynamic_cast<MetadataEntrySingle*>(entry.get());
					parameter.parameterName = nameEntry->getValue().getConstCharPtr();
				}
				else if (entry->getEntryName() == _valuesField)
				{
					auto valueEntry = dynamic_cast<MetadataEntryArray*>(entry.get());
					parameter.values = valueEntry->getValues();
					parameter.typeName = valueEntry->getValues().begin()->getTypeName();
				}
				else
				{
					parameter.metaData[entry->getEntryName()] = entry;
				}
			}

			auto objectList = ExtractMetadataObjects(*parameterDocument);
			for (auto& object : objectList)
			{
				parameter.metaData[object->getEntryName()] = object;
			}
			seriesMetadata.AddParameter(std::move(parameter));
		}
		
		
		const GenericDocument* quantityTopLevel = msmd->getDocument(msmd->getQuantityDocumentName());
		const std::vector<const GenericDocument*> allQuantityDocuments = quantityTopLevel->getSubDocuments();
		for (const GenericDocument* quantityDocument : allQuantityDocuments)
		{
			MetadataQuantity quantity;
			quantity.quantityAbbreviation = quantityDocument->getDocumentName();
			const std::string abbrev = quantity.quantityAbbreviation;
			quantity.quantityIndex = std::stoi(abbrev.substr(abbrev.find("_") + 1, abbrev.size()));
			auto quantityFields = ExtractMetadataFields(*quantityDocument);
			for (std::shared_ptr<MetadataEntry> entry : quantityFields)
			{
				if (entry->getEntryName() == _datatypeField)
				{
					auto typeEntry = dynamic_cast<MetadataEntrySingle*>(entry.get());
					quantity.typeName = typeEntry->getValue().getConstCharPtr();
				}
				else if (entry->getEntryName() == _nameField)
				{
					auto nameEntry = dynamic_cast<MetadataEntrySingle*>(entry.get());
					quantity.quantityName = nameEntry->getValue().getConstCharPtr();
				}
				else
				{
					quantity.metaData[entry->getEntryName()] = entry;
				}
			}

			auto objectList = ExtractMetadataObjects(*quantityDocument);
			for (auto& object : objectList)
			{
				quantity.metaData[object->getEntryName()] = object;
			}
			seriesMetadata.AddQuantity(std::move(quantity));
		}
		measurementCampaign.AddSeriesMetadata(std::move(seriesMetadata));
	}
}