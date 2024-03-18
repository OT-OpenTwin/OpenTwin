#include "MetadataEntityInterface.h"
#include "MetadataEntry.h"
#include "MetadataEntryArray.h"
#include "MetadataEntryObject.h"
#include "MetadataEntrySingle.h"

#include <vector>

MetadataEntityInterface::MetadataEntityInterface(const std::string& ownerServiceName):_ownerServiceName(ownerServiceName)
{
}

MetadataEntityInterface::MetadataEntityInterface()
	:_ownerServiceName("")
{
}

MetadataCampaign MetadataEntityInterface::CreateCampaign(std::shared_ptr<EntityMetadataCampaign> rmd, std::list<std::shared_ptr<EntityMetadataSeries>> msmds)
{
	MetadataCampaign measurementCampaign;
	ExtractCampaignMetadata(measurementCampaign, rmd);
	ExtractSeriesMetadata(measurementCampaign, msmds);
	measurementCampaign.UpdateMetadataOverview();
	return measurementCampaign;
}

MetadataSeries MetadataEntityInterface::CreateSeries(std::shared_ptr<EntityMetadataSeries> seriesMetadataEntity)
{
	std::string entityName = seriesMetadataEntity->getName();
	const std::string name = entityName.substr(entityName.find_last_of("/") + 1);
	MetadataSeries seriesMetadata(name);
	const GenericDocument* topLevel = seriesMetadataEntity->getDocumentTopLevel();
	
	//for (const GenericDocument* subDocument : topLevel->getFields())
	//{
	//	if (subDocument->getDocumentName() != seriesMetadataEntity->getParameterDocumentName())
	//	{
	//		const auto listOfMetadata = ExtractMetadataFields(*subDocument);
	//		for (const auto& metadata : listOfMetadata)
	//		{
	//			seriesMetadata.AddMetadata(metadata);
	//		}
	//	}
	//}

	const GenericDocument* parameterTopLevel = seriesMetadataEntity->getDocument(seriesMetadataEntity->getParameterDocumentName());
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


	const GenericDocument* quantityTopLevel = seriesMetadataEntity->getDocument(seriesMetadataEntity->getQuantityDocumentName());
	const std::vector<const GenericDocument*> allQuantityDocuments = quantityTopLevel->getSubDocuments();
	for (const GenericDocument* quantityDocument : allQuantityDocuments)
	{
		MetadataQuantity quantity;
		quantity.quantityAbbreviation = quantityDocument->getDocumentName();
		const std::string abbrev = quantity.quantityAbbreviation;
		quantity.quantityIndex = std::stoull(abbrev.substr(abbrev.find("_") + 1, abbrev.size()));
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
			else if (entry->getEntryName() == _dataRowsField)
			{
				auto dataRowsEntry = dynamic_cast<MetadataEntrySingle*>(entry.get());
				quantity.dataRows = static_cast<uint32_t>(dataRowsEntry->getValue().getInt32());
			}
			else if (entry->getEntryName() == _dataColumnsField)
			{
				auto dataColumnEntry = dynamic_cast<MetadataEntrySingle*>(entry.get());
				quantity.dataColumns = static_cast<uint32_t>(dataColumnEntry->getValue().getInt32());
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

	return seriesMetadata;
}

void MetadataEntityInterface::StoreCampaign(ot::components::ModelComponent& modelComponent, MetadataCampaign& metaDataCampaign)
{
	EntityMetadataCampaign entityCampaign(modelComponent.createEntityUID(), nullptr, nullptr, nullptr, nullptr, _ownerServiceName);
	for (auto& metadata : metaDataCampaign.getMetaData())
	{
		InsertMetadata(&entityCampaign, metadata.second.get());
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
		EntityMetadataSeries entitySeries(newSeriesMetadata->getSeriesIndex(), nullptr, nullptr, nullptr, nullptr, _ownerServiceName);
		entitySeries.setName(name);
		for (const MetadataParameter& parameter : newSeriesMetadata->getParameter())
		{
			MetadataParameter& parameterForChange = const_cast<MetadataParameter&>(parameter);
			entitySeries.InsertToParameterField(_nameField, { ot::Variable(parameter.parameterName) }, parameter.parameterAbbreviation);
			entitySeries.InsertToParameterField(_valuesField, std::move(parameterForChange.values), parameter.parameterAbbreviation);
			for (auto& metadata : parameter.metaData)
			{
				InsertMetadata(&entitySeries,metadata.second.get());
			}
		}
		for (const MetadataQuantity& quantity : newSeriesMetadata->getQuantities())
		{
			entitySeries.InsertToQuantityField(_nameField, { ot::Variable(quantity.quantityName) }, quantity.quantityAbbreviation);
			entitySeries.InsertToQuantityField(_datatypeField, { ot::Variable(quantity.typeName) }, quantity.quantityAbbreviation);
			entitySeries.InsertToQuantityField(_dataRowsField, { ot::Variable(static_cast<int32_t>(quantity.dataRows)) }, quantity.quantityAbbreviation);
			entitySeries.InsertToQuantityField(_dataColumnsField, { ot::Variable(static_cast<int32_t>(quantity.dataColumns)) }, quantity.quantityAbbreviation);

			for (auto& metadata : quantity.metaData)
			{
				InsertMetadata(&entitySeries, metadata.second.get());
			}
		}
		for (auto& metadata : newSeriesMetadata->getMetadata())
		{
			InsertMetadata(&entitySeries, metadata.second.get());
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
		MetadataSeries seriesMetadata = CreateSeries(msmd);
		measurementCampaign.AddSeriesMetadata(std::move(seriesMetadata));
	}
}

void MetadataEntityInterface::InsertMetadata(EntityWithDynamicFields* entity, MetadataEntry* metadata, const std::string documentName)
{
	auto singleEntry = dynamic_cast<MetadataEntrySingle*>(metadata);
	if(singleEntry != nullptr)
	{
		entity->InsertInField(singleEntry->getEntryName(), { singleEntry->getValue() }, documentName);
	}
	else
	{
		auto arrayEntry = dynamic_cast<MetadataEntryArray*>(metadata);
		if (arrayEntry != nullptr)
		{
			entity->InsertInField(arrayEntry->getEntryName(), arrayEntry->getValues(), documentName);
		}
		else
		{
			auto objectEntry = dynamic_cast<MetadataEntryObject*>(metadata);
			assert(objectEntry != nullptr);
			const std::string& name = documentName + "/" + objectEntry->getEntryName();
			for (auto entry : objectEntry->getEntries())
			{
				InsertMetadata(entity, entry.get(), name);
			}
		}

	}


	
}
