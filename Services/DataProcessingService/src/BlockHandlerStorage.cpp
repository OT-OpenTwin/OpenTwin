// @otlicense
// File: BlockHandlerStorage.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "BlockHandler.h"
#include "BlockHandlerStorage.h"
#include "Application.h"

#include "OTCore/EntityName.h"
#include "OTCore/FolderNames.h"
#include "OTCore/TimeFormatter.h"
#include "OTCore/Variable/JSONToVariableConverter.h"
#include "OTCore/Variable/ExplicitStringValueConverter.h"
#include "OTGui/Plot/Plot1DCfg.h"
#include "OTGui/Plot/Plot1DCurveCfg.h"
#include "OTGui/Painter/PainterRainbowIterator.h"
#include "OTCommunication/ActionTypes.h"

#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelEntities/NewModelStateInfo.h"
#include "OTModelEntities/EntityResult1DPlot.h"

#include "OTCore/MetadataHandle/MetadataQuantity.h"
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionCurve.h"
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionMatrix.h"
#include "OTResultDataAccess/ResultCollection/ResultCollectionMetadataAccess.h"
#include "OTResultDataAccess/CurveFactory.h"

#include <map>

BlockHandlerStorage::BlockHandlerStorage(EntityBlockStorage* blockEntityStorage, const HandlerMap& handlerMap)
	:BlockHandler(blockEntityStorage,handlerMap),m_blockEntityStorage(blockEntityStorage)
{
	m_allDataInputs = m_blockEntityStorage->getInputNames();
	m_seriesMetadataInput = m_blockEntityStorage->getSeriesConnectorName();
	m_createPlot = m_blockEntityStorage->getCreatePlot();
	m_plotName = m_blockEntityStorage->getPlotName();
}

bool BlockHandlerStorage::executeSpecialized()
{

	if (allInputsAvailable())
	{
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
		ResultCollectionExtender resultCollectionExtender(collectionName, *modelComponent);
		resultCollectionExtender.registerCallbacks(
			ot::EntityCallbackBase::Callback::Properties |
			ot::EntityCallbackBase::Callback::Selection,
			Application::instance()->getServiceName()
		);
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
						//Until here we check if the format of the pipeline data is as expected
						auto entryObject = entry.GetObject();
						
						// The quantity and its values can be extracted directly from the document. However, the parameter are potentially limitless.
						// To improve the users ability to debug the issue, we need to identify all parameter that are not listed in the metadata, too.
						// Thus we cannot simply extract the known parameter from the document. We need to iterate through the fields of the document, such that we can notify the user
						// if we found a field for which there are no metadata in the parameter. In this iteration we exclude the value and quantity fields
						// However, it is better to deal with the quantities separate, since the quantity determines how many value fields exist in the document (matrices have multiple)
						std::list<std::string> alreadyDealtWithFields;
						// First we extract the quantity
						const auto& quantityLabelEntry = entryObject[MetadataQuantity::getFieldName().c_str()];
						if (!quantityLabelEntry.IsString())
						{
							throw std::exception(("The quantity field is not described by a string. Content:  " + ot::json::toJson(quantityLabelEntry)).c_str());
						}
						alreadyDealtWithFields.push_back(MetadataQuantity::getFieldName());
						
						// We ignore the series entry entirely. Any series information is given in the metadata
						alreadyDealtWithFields.push_back(MetadataSeries::getFieldName());

						const std::string quantityLabel = std::string(quantityLabelEntry.GetString());
						// Now we check if the quantity was defined in the metadata
						auto pipelineQuantityByLabel = pipelineCampaignQuantitiesByLabel.find(quantityLabel);
						if (pipelineQuantityByLabel != pipelineCampaignQuantitiesByLabel.end())
						{
							auto datasetDescription = datasetDescriptionByQuantityLabel.find(quantityLabel);
							MetadataQuantity* pipelineQuantity = pipelineQuantityByLabel->second;

							// Here we check if we need to create a new dataset descriptions, since each description holds a single quantity
							if (datasetDescription == datasetDescriptionByQuantityLabel.end())
							{
								DatasetDescription newDatasetDescription;
								std::unique_ptr<QuantityDescription> quantityDescription;
								if (pipelineQuantity->dataDimensions.size() > 1) // We got a matrix
								{
									ot::MatrixEntryPointer matrixDimensions;
									matrixDimensions.setRow(pipelineQuantity->dataDimensions[0]);
									matrixDimensions.setColumn(pipelineQuantity->dataDimensions[1]);

									quantityDescription.reset(new QuantityDescriptionMatrix(matrixDimensions));
								}
								else
								{
									quantityDescription.reset(new QuantityDescriptionCurve());

								}

								quantityDescription->setMetadataQuantity(*pipelineQuantity);
								// Now we reset the ids and dependencies of the quantity. They may not be the same anymore.
								// The Index is set when the series is created and a new label and index are given to this quantity, if the quantity already exists in the campaign 
								// but differs in its attributes from the quantity here passed through the pipeline. For more info see createSeries() and the according addCampaignContextDataToQuantities method
								// in OTResultDataAccess::ResultCollectionExtender
								quantityDescription->getMetadataQuantity().dependingParameterIds.clear();
								quantityDescription->getMetadataQuantity().quantityIndex = 0;

								newDatasetDescription.setQuantityDescription(quantityDescription.release());
								datasetDescriptionByQuantityLabel[quantityLabel] = std::move(newDatasetDescription);
								datasetDescription = datasetDescriptionByQuantityLabel.find(quantityLabel);
							}


							const std::string& dataType = ot::TypeNames::getDoubleTypeName(); //pipelineQuantity->m_tupleDescription.getDataType();
							// Now we pull the data values from the document
							if (pipelineQuantity->dataDimensions.size() > 1) // Here the data is a matrix
							{
								QuantityDescriptionMatrix* quantityDescription = dynamic_cast<QuantityDescriptionMatrix*>(datasetDescription->second.getQuantityDescription());
								uint64_t numberOfEntries = quantityDescription->getNumberOfEntries();
								
								ot::MatrixEntryPointer matrixDimensions;
								matrixDimensions.setRow(pipelineQuantity->dataDimensions[0]);
								matrixDimensions.setColumn(pipelineQuantity->dataDimensions[1]);

								std::list<ot::Variable> values;
								for (int i = 0; i < numberOfEntries; i++)
								{
									const std::string valuesFieldName = QuantityContainer::getFieldName(i);
									alreadyDealtWithFields.push_back(valuesFieldName);
									const ot::JsonValue& matrixEntry =	entryObject[valuesFieldName.c_str()];
									if (dataType != "")
									{
										if (!ot::JSONToVariableConverter::typeIsCompatible(matrixEntry, dataType))
										{
											throw std::exception(("The data type of: " + quantityLabel + " is inconsistent with the data type mentioned in its metadata.").c_str());
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
								ot::GenericDataStructMatrix matrixValues(matrixDimensions);
								matrixValues.setValues(values);
								quantityDescription->addToValues(matrixValues);
							}
							else
							{
								const ot::JsonValue& quantityValue = entryObject[QuantityContainer::getFieldName().c_str()];
								alreadyDealtWithFields.push_back(QuantityContainer::getFieldName());
								QuantityDescriptionCurve * curve = dynamic_cast<QuantityDescriptionCurve*>(datasetDescription->second.getQuantityDescription());
								if (dataType != "")
								{
									if (!ot::JSONToVariableConverter::typeIsCompatible(quantityValue, dataType))
									{
										throw std::exception(("The data type of: " + quantityLabel + " is inconsistent with the data type mentioned in its metadata.").c_str());
									}
									ot::Variable value = converter(quantityValue, dataType);
									curve->addDatapoint(std::move(value));
								}
								else
								{
									ot::Variable value = converter(quantityValue);
									curve->addDatapoint(std::move(value));
								}
							}
						}
						else
						{
							throw std::exception(("The quantity:  " + quantityLabel + " was not defined in the metadata of the data pipeline.").c_str());
						}

						// Now we iterate over all entries of the document to deal with all not-quantity-related fields
						for (auto& field : entryObject)
						{
							const std::string key = field.name.GetString();
							if (std::find(alreadyDealtWithFields.begin(), alreadyDealtWithFields.end(), key) == alreadyDealtWithFields.end())
							{
								auto occurringParameterByLabel = occurringParametersByLabel.find(key);

								if (occurringParameterByLabel == occurringParametersByLabel.end())
								{
									auto campaignParameterByLabel = pipelineCampaignParametersByLabel.find(key);
									if (campaignParameterByLabel == pipelineCampaignParametersByLabel.end())
									{
										throw std::exception(("The parameter " + key + " is not defined in the metadata.").c_str());
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
								const std::string& typeName = param.typeName;
								const ot::JsonValue& fieldValue = field.value;
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
			
			ot::UID seriesID = resultCollectionExtender.buildSeriesMetadata(datasetDescr, seriesName);
			
			SolverReport::instance().addToContentAndDisplay("Storing data into series: " + seriesName + ".\n", _uiComponent);

			auto startTimePoint = std::chrono::high_resolution_clock::now();
			for (DatasetDescription& dsd : datasetDescr)
			{
				resultCollectionExtender.processDataPoints(&dsd, seriesID);
			}
			resultCollectionExtender.storeCampaignChanges();
			auto endTimePoint = std::chrono::high_resolution_clock::now();

			const std::string duration = TimeFormatter::formatDuration(startTimePoint, endTimePoint);
			SolverReport::instance().addToContentAndDisplay("Data storage completed in " +duration + ".\n", _uiComponent);

			if (m_createPlot)
			{
				const MetadataSeries* series = resultCollectionExtender.findMetadataSeries(seriesID);
				
				ot::Plot1DCurveCfg curveConfig;
				auto painter = colourIt.getNextPainter();
				curveConfig.setLinePenPainter(painter.release());
				// Question: Which of the quantities ?
				//CurveFactory::addToConfig(*series, ,curveConfig,Application::instance(), &resultCollectionExtender);
				assert(false); // ToDo
				EntityResult1DCurve newCurve(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
				newCurve.registerCallbacks(
					ot::EntityCallbackBase::Callback::Properties |
					ot::EntityCallbackBase::Callback::Selection,
					Application::instance()->getServiceName()
				);

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
			EntityResult1DPlot newPlot(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
			newPlot.setName(plotName);
			newPlot.registerCallbacks(
				ot::EntityCallbackBase::Callback::Properties |
				ot::EntityCallbackBase::Callback::Selection,
				Application::instance()->getServiceName()
			);

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

std::string BlockHandlerStorage::getBlockType() const
{
	return "Storage Block";
}
