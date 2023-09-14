#include "MeasurementCampaignFactory.h"
#include "MetadataEntry.h"
#include "MetadataEntryArray.h"
#include "MetadataEntryObject.h"
#include "MetadataEntrySingle.h"

#include <vector>

MeasurementCampaign MeasurementCampaignFactory::Create(std::shared_ptr<EntityResearchMetadata> rmd, std::list<std::shared_ptr<EntityMeasurementMetadata>> msmds)
{
	MeasurementCampaign measurementCampaign;
	ExtractCampaignMetadata(measurementCampaign, rmd);
	ExtractSeriesMetadata(measurementCampaign, msmds);
	BuildUpOverviewLists(measurementCampaign);
	return measurementCampaign;
}

void MeasurementCampaignFactory::ExtractCampaignMetadata(MeasurementCampaign& measurementCampaign, std::shared_ptr<EntityResearchMetadata> rmd)
{
	const GenericDocument* topLevel= rmd->getDocumentTopLevel();
	auto fieldList = ExtractMetadataFields(*topLevel);
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

std::list<std::shared_ptr<MetadataEntry>> MeasurementCampaignFactory::ExtractMetadataObjects(const GenericDocument& document)
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

std::list<std::shared_ptr<MetadataEntry>> MeasurementCampaignFactory::ExtractMetadataFields(const GenericDocument& document)
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

void MeasurementCampaignFactory::ExtractSeriesMetadata(MeasurementCampaign& measurementCampaign, std::list<std::shared_ptr<EntityMeasurementMetadata>> msmds)
{
	for (auto msmd : msmds)
	{
		SeriesMetadata seriesMetadata (msmd->getName());
		
		const GenericDocument* parameterTopLevel = msmd->getDocument(msmd->getParameterDocumentName());
		const std::vector<const GenericDocument*> allParameterDocuments = parameterTopLevel->getSubDocuments();
		for (const GenericDocument* parameterDocument : allParameterDocuments)
		{
			MetadataParameter parameter;
			parameter.parameterAbbreviation = parameterDocument->getDocumentName();
			auto parameterFields = ExtractMetadataFields(*parameterDocument);
			for (std::shared_ptr<MetadataEntry> entry : parameterFields)
			{
				if (entry->getEntryName() == "Name")
				{
					auto nameEntry = dynamic_cast<MetadataEntrySingle*>(entry.get());
					parameter.parameterName = nameEntry->getValue().getConstCharPtr();
				}
				else if (entry->getEntryName() == "Value")
				{
					auto valueEntry = dynamic_cast<MetadataEntryArray*>(entry.get());
					parameter.values = valueEntry->getValues();
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
			
			auto quantityFields = ExtractMetadataFields(*quantityDocument);
			for (std::shared_ptr<MetadataEntry> entry : quantityFields)
			{
				if (entry->getEntryName() == "Datatype")
				{
					auto typeEntry = dynamic_cast<MetadataEntrySingle*>(entry.get());
					quantity.typeName = typeEntry->getValue().getConstCharPtr();
				}
				else if (entry->getEntryName() == "Name")
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



void MeasurementCampaignFactory::BuildUpOverviewLists(MeasurementCampaign& measurementCampaign)
{
	const std::list<SeriesMetadata> allSeriesMetadata = measurementCampaign.getSeriesMetadata();
	std::map < std::string, MetadataParameter> parameterOverview;
	std::map < std::string, MetadataQuantity > quantityOverview;

	for (auto& seriesMetadata : allSeriesMetadata)
	{
		auto allParameter =	seriesMetadata.getParameter();
		for (auto& parameter : allParameter)
		{
			if (parameterOverview.find(parameter.parameterName) == parameterOverview.end())
			{
				parameterOverview[parameter.parameterName] = parameter;
			}
			else
			{
				std::list<ot::Variable>& values = parameterOverview[parameter.parameterName].values;
				values.splice(values.begin(), parameter.values);
			}
		}

		auto allQuanties = seriesMetadata.getQuantities();
		for (auto& quantity : allQuanties)
		{
			if(quantityOverview.find(quantity.quantityName) == quantityOverview.end())
			{
				quantityOverview[quantity.quantityName] = quantity;
			}
		}
	}
	measurementCampaign.setParameterOverview(std::move(parameterOverview));
	measurementCampaign.setQuantityOverview(std::move(quantityOverview));
}


