#include "BlockHandler.h"
#include "BlockHandlerStorage.h"
#include "Application.h"

#include "QuantityDescriptionMatrix.h"
#include "QuantityDescriptionCurve.h"
#include "MetadataQuantity.h"
#include <map>
#include "OTCore/FolderNames.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/ExplicitStringValueConverter.h"
#include "ResultCollectionMetadataAccess.h"

BlockHandlerStorage::BlockHandlerStorage(EntityBlockStorage* blockEntityStorage, const HandlerMap& handlerMap)
	:BlockHandler(blockEntityStorage,handlerMap),m_blockEntityStorage(blockEntityStorage)
{
	m_allDataInputs = m_blockEntityStorage->getInputNames();
	m_seriesMetadataInput = m_blockEntityStorage->getSeriesConnectorName();
}

BlockHandlerStorage::~BlockHandlerStorage()
{
	
}

bool BlockHandlerStorage::executeSpecialized()
{

	if (allInputsAvailable())
	{
		_uiComponent->displayMessage("Executing Storage Block: " + m_blockName);

		auto& classFactory = Application::instance()->getClassFactory();
		const auto modelComponent = Application::instance()->modelComponent();
		const std::string collectionName = Application::instance()->getCollectionName();
		ResultCollectionExtender resultCollectionExtender(collectionName, *modelComponent, &classFactory, OT_INFO_SERVICE_TYPE_DataProcessingService);
				
		ot::JSONToVariableConverter converter;
		for (const std::string portName : m_allDataInputs)
		{

			const std::string portInvalidData = "Port " + portName + " received data in an invalid format. The input needs to be an array of json objects.";

			auto pipelineByName = m_dataPerPort.find(portName);
			PipelineData* dataPipeline = pipelineByName->second;

			const MetadataCampaign* campaign = dataPipeline->getMetadataCampaign();
			const std::map <std::string, MetadataQuantity*>& quantitiesByLabel = campaign->getMetadataQuantitiesByLabel();
			const std::map<std::string, MetadataParameter*>& campaignParametersByLabel = campaign->getMetadataParameterByLabel();

			std::map<std::string, DatasetDescription> datasetDescriptionByQuantityLabel;
			std::map < std::string, MetadataParameter> occurringParametersByLabel;

			ot::JsonValue& dataEntries = dataPipeline->getData();
			
			if (dataEntries.IsArray())
			{
				auto dataArray = dataEntries.GetArray();
				for (const auto& entry : dataArray)
				{
					if (entry.IsObject())
					{
						//Until here we check if the format is as expected
						auto entryObject = entry.GetObject();
						for (auto& field : entryObject) //Here we extract the fields of the documents and store them as quantity or parameter, depending on the metadata definitions.
						{
							const std::string key = field.name.GetString();
							const ot::JsonValue& fieldValue = field.value;

							auto quantityByLabel =	quantitiesByLabel.find(key);
							if (quantityByLabel != quantitiesByLabel.end())
							{
								auto datasetDescription = datasetDescriptionByQuantityLabel.find(key);
								MetadataQuantity* quantity = quantityByLabel->second;
								if (datasetDescription == datasetDescriptionByQuantityLabel.end())
								{
									DatasetDescription newDatasetDescription;
									std::unique_ptr<QuantityDescriptionCurve> curve(new QuantityDescriptionCurve());
									curve->setMetadataQuantity(*quantity);
									//Now we reset the ids and dependencies of the quantity. They may not be the same anymore.
									curve->getMetadataQuantity().dependingParameterIds.clear();
									curve->getMetadataQuantity().quantityIndex = 0;
									for (auto& valueDescription : curve->getMetadataQuantity().valueDescriptions)
									{
										valueDescription.quantityIndex = 0;
									}
									newDatasetDescription.setQuantityDescription(curve.release());
									datasetDescriptionByQuantityLabel[key] = std::move(newDatasetDescription);
									datasetDescription = datasetDescriptionByQuantityLabel.find(key);
								}
								const std::string& dataType = quantity->valueDescriptions.begin()->dataTypeName;
								QuantityDescriptionCurve* curve = dynamic_cast<QuantityDescriptionCurve*>(datasetDescription->second.getQuantityDescription());
								if (dataType != "")
								{
									ot::Variable value = converter(fieldValue, dataType);
									curve->addDatapoint(std::move(value));
								}
								else
								{
									ot::Variable value = converter(fieldValue);
									curve->addDatapoint(std::move(value));
								}
							}
							else
							{
								auto occurringParameterByLabel = occurringParametersByLabel.find(key);
								if (occurringParameterByLabel == occurringParametersByLabel.end())
								{
									auto campaignParameterByLabel = campaignParametersByLabel.find(key);
									if (campaignParameterByLabel == campaignParametersByLabel.end())
									{
										//Error case
										assert(false);
									}
									else
									{
										MetadataParameter campaignParameter= *campaignParameterByLabel->second;
										campaignParameter.values.clear();
										occurringParametersByLabel[key] = std::move(campaignParameter);
										occurringParameterByLabel = occurringParametersByLabel.find(key);
									}
								}

								MetadataParameter& param = occurringParameterByLabel->second;
								const std::string typeName = param.typeName;
								if (typeName != "")
								{
									param.values.push_back(converter(fieldValue, typeName));
								}
								else
								{
									param.values.push_back(converter(fieldValue));
								}
							}
						
						}
					}
					else
					{
						throw std::exception(portInvalidData.c_str());
					}
				}
			}
			else
			{
				throw std::exception(portInvalidData.c_str());
			}

			std::list <std::shared_ptr<ParameterDescription>> paramDescriptions;
			for (auto& parameterByLabel : occurringParametersByLabel)
			{
				MetadataParameter& parameter =	parameterByLabel.second;
				
				bool constInDataset = false; // Here we ignore the possibility of const parameter in a dataset, because we actually got already all our parameter values.
				
				std::shared_ptr<ParameterDescription> paramDescription(new ParameterDescription(parameter, constInDataset));
				paramDescriptions.push_back(paramDescription);
			}

			std::list<DatasetDescription> datasetDescr;
			for (auto& datasetByLabel : datasetDescriptionByQuantityLabel)
			{
				DatasetDescription& dataset = datasetByLabel.second;
				dataset.addParameterDescriptions(paramDescriptions);
				datasetDescr.push_back(std::move(dataset));

			}
			const std::string seriesName = "Test";
			std::list<std::shared_ptr<MetadataEntry>> metadata;
			ot::UID seriesID = resultCollectionExtender.buildSeriesMetadata(datasetDescr, seriesName,metadata);
			for (DatasetDescription& dsd : datasetDescr)
			{
				resultCollectionExtender.processDataPoints(&dsd, seriesID);
			}
			resultCollectionExtender.storeCampaignChanges();
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool BlockHandlerStorage::allInputsAvailable()
{
	for (auto& input: m_allDataInputs)
	{
		if (m_dataPerPort.find(input) == m_dataPerPort.end())
		{
			return false;
		}
	}
	return true;
}

std::list<DatasetDescription> BlockHandlerStorage::createDatasets()
{
	
	std::list<DatasetDescription> allDataDescriptions;

	return allDataDescriptions;
}