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

#include "OTGui/Plot1DCfg.h"
#include "OTGui/Plot1DCurveCfg.h"
#include "OTCore/EntityName.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/PainterRainbowIterator.h"
#include "CurveFactory.h"
#include "NewModelStateInfo.h"
#include "EntityResult1DPlot.h"
#include "OTModelAPI/ModelServiceAPI.h"


BlockHandlerStorage::BlockHandlerStorage(EntityBlockStorage* blockEntityStorage, const HandlerMap& handlerMap)
	:BlockHandler(blockEntityStorage,handlerMap),m_blockEntityStorage(blockEntityStorage)
{
	m_allDataInputs = m_blockEntityStorage->getInputNames();
	m_seriesMetadataInput = m_blockEntityStorage->getSeriesConnectorName();
	m_createPlot = m_blockEntityStorage->getCreatePlot();
	m_plotName = m_blockEntityStorage->getPlotName();
}

BlockHandlerStorage::~BlockHandlerStorage()
{
	
}

bool BlockHandlerStorage::executeSpecialized()
{

	if (allInputsAvailable())
	{
		_uiComponent->displayMessage("Executing Storage Block: " + m_blockName + "\n");

		ot::PainterRainbowIterator colourIt;
		std::string plotName = "Plots/";
		auto blockName = ot::EntityName::getSubName(m_blockName);
		assert(blockName.has_value());
		std::string blockNameShort = blockName.value();
		if (!m_plotName.empty())
		{
			plotName += m_plotName;
		}
		else
		{
			
			plotName += blockNameShort;
		}
		ot::NewModelStateInfo modelStateInformation;

		const auto modelComponent = Application::instance()->getModelComponent();
		const std::string collectionName = Application::instance()->getCollectionName();
		ResultCollectionExtender resultCollectionExtender(collectionName, *modelComponent, OT_INFO_SERVICE_TYPE_DataProcessingService);
		resultCollectionExtender.setSaveModel(!m_createPlot); //If a plot shall be added as well, we create more entities later on.

		ot::JSONToVariableConverter converter;
		for (const std::string portName : m_allDataInputs)
		{

			const std::string portInvalidData = "Port " + portName + " received data in an invalid format. The input needs to be an array of json objects.";

			auto pipelineByName = m_dataPerPort.find(portName);
			if (pipelineByName == m_dataPerPort.end() || pipelineByName->second == nullptr)
			{
				throw std::exception(("No data found in port: " + portName).c_str());
			}
			PipelineData* dataPipeline = pipelineByName->second;

			const MetadataCampaign* pipelineCampaign = dataPipeline->getMetadataCampaign();
			if (pipelineCampaign == nullptr)
			{
				throw std::exception(("Metadata are missing in port: " + portName).c_str());
			}
			auto pipelineCampaignQuantitiesByLabel = pipelineCampaign->getMetadataQuantitiesByLabel();
			auto pipelineCampaignParametersByLabel = pipelineCampaign->getMetadataParameterByLabel();

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
								auto pipelineQuantityByLabel = pipelineCampaignQuantitiesByLabel.find(key);
								if (pipelineQuantityByLabel != pipelineCampaignQuantitiesByLabel.end())
								{
									auto datasetDescription = datasetDescriptionByQuantityLabel.find(key);
									MetadataQuantity* pipelineQuantity = pipelineQuantityByLabel->second;
									if (datasetDescription == datasetDescriptionByQuantityLabel.end())
									{
										DatasetDescription newDatasetDescription;
										std::unique_ptr<QuantityDescription> quantityDescription;
										if (pipelineQuantity->dataDimensions.size() > 1) // We got a matrix
										{
											ot::MatrixEntryPointer matrixDimensions;
											matrixDimensions.m_row = pipelineQuantity->dataDimensions[0];
											matrixDimensions.m_column = pipelineQuantity->dataDimensions[1];

											quantityDescription.reset(new QuantityDescriptionMatrix(matrixDimensions));
										}
										else
										{
											quantityDescription.reset(new QuantityDescriptionCurve());

										}

										quantityDescription->setMetadataQuantity(*pipelineQuantity);
										//Now we reset the ids and dependencies of the quantity. They may not be the same anymore.
										quantityDescription->getMetadataQuantity().dependingParameterIds.clear();
										quantityDescription->getMetadataQuantity().quantityIndex = 0;
										for (auto& valueDescription : quantityDescription->getMetadataQuantity().valueDescriptions)
										{
											valueDescription.quantityIndex = 0;
										}
										newDatasetDescription.setQuantityDescription(quantityDescription.release());
										datasetDescriptionByQuantityLabel[key] = std::move(newDatasetDescription);
										datasetDescription = datasetDescriptionByQuantityLabel.find(key);
									}

									const std::string& dataType = pipelineQuantity->valueDescriptions.begin()->dataTypeName;
									if (pipelineQuantity->dataDimensions.size() > 1) // Here the data is a matrix
									{
										QuantityDescriptionMatrix* matrix = dynamic_cast<QuantityDescriptionMatrix*>(datasetDescription->second.getQuantityDescription());
										ot::MatrixEntryPointer matrixDimensions;
										matrixDimensions.m_row = pipelineQuantity->dataDimensions[0];
										matrixDimensions.m_column = pipelineQuantity->dataDimensions[1];

										ot::GenericDataStructMatrix matrixValues(matrixDimensions);
										ot::ConstJsonArray linearMatrix = fieldValue.GetArray();
										std::list<ot::Variable> values;
										for (auto& matrixEntry : linearMatrix)
										{
											if (dataType != "")
											{
												if (!ot::JSONToVariableConverter::typeIsCompatible(matrixEntry, dataType))
												{
													throw std::exception(("The data type of: " + key + " is inconsistent with the data type mentioned in its metadata.").c_str());
												}
												const ot::Variable value = converter(matrixEntry, dataType);
												values.push_back(value);
											}
											else
											{
												const ot::Variable value = converter(matrixEntry);
												values.push_back(value);
											}
										}
										matrixValues.setValues(values);
										matrix->addToValues(matrixValues);
									}
									else
									{
										QuantityDescriptionCurve* curve = dynamic_cast<QuantityDescriptionCurve*>(datasetDescription->second.getQuantityDescription());
										if (dataType != "")
										{
											if (!ot::JSONToVariableConverter::typeIsCompatible(fieldValue, dataType))
											{
												throw std::exception(("The data type of: " + key + " is inconsistent with the data type mentioned in its metadata.").c_str());
											}
											ot::Variable value = converter(fieldValue, dataType);
											curve->addDatapoint(std::move(value));
										}
										else
										{
											ot::Variable value = converter(fieldValue);
											curve->addDatapoint(std::move(value));
										}
									}
								}
								else
								{
									auto occurringParameterByLabel = occurringParametersByLabel.find(key);
									if (occurringParameterByLabel == occurringParametersByLabel.end())
									{
										auto campaignParameterByLabel = pipelineCampaignParametersByLabel.find(key);
										if (campaignParameterByLabel == pipelineCampaignParametersByLabel.end())
										{
											throw std::exception(("Field " + key + " is neither defined as quantity nor as parameter in the campaign metadata.").c_str());
										}
										else
										{
											MetadataParameter campaignParameter = *campaignParameterByLabel->second;
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

			
			const std::string seriesName = ot::FolderNames::DatasetFolder + "/" + blockNameShort;
			std::list<std::shared_ptr<MetadataEntry>> metadata;
			ot::UID seriesID = resultCollectionExtender.buildSeriesMetadata(datasetDescr, seriesName,metadata);
			
			for (DatasetDescription& dsd : datasetDescr)
			{
				resultCollectionExtender.processDataPoints(&dsd, seriesID);
			}
			resultCollectionExtender.storeCampaignChanges();

			if (m_createPlot)
			{
				const MetadataSeries* series = resultCollectionExtender.findMetadataSeries(seriesID);
				
				ot::Plot1DCurveCfg curveConfig;
				auto painter = colourIt.getNextPainter();
				curveConfig.setLinePenPainter(painter.release());
				CurveFactory::addToConfig(*series, curveConfig);

				EntityResult1DCurve newCurve(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, Application::instance()->getServiceName());

				const std::string fullNameSeries = series->getName();
				std::optional<std::string> shortNameSeries = ot::EntityName::getSubName(fullNameSeries);
				assert(shortNameSeries.has_value());

				newCurve.setName(plotName + "/"+ shortNameSeries.value());
				newCurve.createProperties();
				newCurve.setCurve(curveConfig);
				newCurve.storeToDataBase();

				modelStateInformation.addTopologyEntity(newCurve);
			}
		}
		
		if (m_createPlot)
		{
			EntityResult1DPlot newPlot(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, Application::instance()->getServiceName());
			newPlot.setName(plotName);

			ot::Plot1DCfg plotCfg;
			const std::string shortName = plotName.substr(plotName.find_last_of("/") + 1);
			plotCfg.setTitle(shortName);
			newPlot.createProperties();
			newPlot.setPlot(plotCfg);
			newPlot.storeToDataBase();
			modelStateInformation.addTopologyEntity(newPlot);

			//Store state
			ot::ModelServiceAPI::addEntitiesToModel(modelStateInformation, "Created new plot for existing series metadata", true, true);
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