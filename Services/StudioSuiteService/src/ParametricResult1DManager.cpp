// @otlicense
// File: ParametricResult1DManager.cpp
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

#include "ParametricResult1DManager.h"
#include "Result1DManager.h"
#include "Application.h"
#include "DataBase.h"

#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/FolderNames.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include "ResultCollectionExtender.h"
#include "MetadataSeries.h"
#include "MetadataParameter.h"
#include "MetadataEntrySingle.h"

#include "QuantityDescription.h"
#include "QuantityDescriptionCurve.h"
#include "QuantityDescriptionCurveComplex.h"
#include "QuantityDescriptionSParameter.h"

#include <boost/algorithm/string.hpp>
#include "ValueFormatSetter.h"

ParametricResult1DManager::ParametricResult1DManager(Application *app) :
	m_resultFolderName(ot::FolderNames::DatasetFolder),
	m_application(app)
{
	setModelComponent(m_application->getModelComponent());
	setUIComponent(m_application->getUiComponent());
}

ParametricResult1DManager::~ParametricResult1DManager()
{

}

void ParametricResult1DManager::clear()
{
	// We delete all previous result data (series), since there was a non-parametric change
	std::list<std::string> resultEntity{ m_resultFolderName + "/1D Results" };

	std::string resultSeriesName = ot::FolderNames::DatasetFolder + "/" + m_seriesNameBase;
	resultEntity.push_back(resultSeriesName);

	ot::ModelServiceAPI::deleteEntitiesFromModel(resultEntity, false);
}

void ParametricResult1DManager::extractData(Result1DManager& result1DManager)
{
	// We add the data in result1DManager to the parametric storage
	std::list<int> runIDList = result1DManager.getRunIDList();
	if (runIDList.empty()) return;

	std::string runIDLabel = determineRunIDLabel(runIDList);

	//First, we assemble all new metadata in a new series metadata entity
	
	// Now we process the different types of data entries
	//const std::vector<std::string> categories{ "1D Results/Balance", "1D Results/Energy", "1D Results/Port signals", "1D Results/Power","1D Results/Reference Impedance" };

	for (int runID : runIDList)
	{
		RunIDContainer* runIDContainer = result1DManager.getContainer(runID);
		assert(runIDContainer != nullptr);

		for (const std::string& category : runIDContainer->getListOfCategories())
		{
			if (category == "1D Results/S-Parameters")
			{
				DatasetDescription sparameterDescriptions;
				if (extractDataDescriptionSParameter(category, runIDContainer, runID, sparameterDescriptions))
				{
					m_allDataDescriptions.push_back(std::move(sparameterDescriptions));
				}
			}
			else
			{
				std::list<DatasetDescription>	curveDescriptions = extractDataDescriptionCurve(category, runIDContainer, runID);
				m_allDataDescriptions.splice(m_allDataDescriptions.end(), std::move(curveDescriptions));
			}
		}
	}
}

void ParametricResult1DManager::storeDataInResultCollection()
{

	//First we access the result collection of the current project
	std::string collectionName = DataBase::instance().getCollectionName();

	ResultCollectionExtender resultCollectionExtender(collectionName, *m_application->getModelComponent(), OT_INFO_SERVICE_TYPE_STUDIOSUITE);
	resultCollectionExtender.setSaveModel(false);

	std::string seriesName = CreateNewUniqueTopologyName(m_resultFolderName, m_seriesNameBase);
	
	std::list<std::shared_ptr<MetadataEntry>> seriesMetadata;
	uint64_t seriesMetadataIndex = resultCollectionExtender.buildSeriesMetadata(m_allDataDescriptions, seriesName, seriesMetadata);
	resultCollectionExtender.storeCampaignChanges();
	//Now we store all data points in the result collection
	for (DatasetDescription& dataDescription : m_allDataDescriptions)
	{
		try
		{
			resultCollectionExtender.processDataPoints(&dataDescription, seriesMetadataIndex);
		}
		catch (std::exception e)
		{
			OT_LOG_E("Unable to import data: " + dataDescription.getQuantityDescription()->getName() + " (" + e.what() + ")");
		}
	}
}

std::string ParametricResult1DManager::determineRunIDLabel(std::list<int> &runIDList)
{
	assert(!runIDList.empty());

	int minRunID = runIDList.front();
	int maxRunID = runIDList.back();

	if (minRunID == maxRunID)
	{
		return "Run " + std::to_string(minRunID);
	}

	return "Run " + std::to_string(minRunID) + "-" + std::to_string(maxRunID);
}

std::list<DatasetDescription>  ParametricResult1DManager::extractDataDescriptionCurve(const std::string& _category, RunIDContainer* _runIDContainer, int _runID)
{
	std::list<DatasetDescription> allCurveDescriptions;
	std::map<std::string, Result1DData*> categoryResults = _runIDContainer->getResultsForCategory(_category);
	if (!categoryResults.empty())
	{
		//We determine the parameter, which are shared by all curves.
		auto allParameterDescriptions = extractParameterDescription(_category, _runIDContainer, _runID);

		//Now we create the individual curve descriptions
		for (auto& curve : categoryResults)
		{
			if (!curve.second->getXValues().empty())   // Some 1D Results data have no x values assigned. We cannot deal with such data here
			{
				DatasetDescription newCurveDescription;

				newCurveDescription.addParameterDescriptions(allParameterDescriptions);

				Result1DData* curveData = curve.second;

				std::string quantityUnit, quantityName;
				//parseAxisLabel(curveData->getYLabel(), quantityName, quantityUnit);
				//quantityName= quantityName;

				//std::string prefix = curve.first.substr(_category.size() + 1);
				quantityUnit = "";
				quantityName = curve.first;
				QuantityDescription* quantityDescription = nullptr;
				//quantityDescription->setName(prefix);

				const bool hasRealValues = !curveData->getYreValues().empty();
				const bool hasImValue = !curveData->getYimValues().empty();
				ValueFormatSetter valueFormatSetter;

				//Values are either complex or real ? Plain imaginary values?
				if (hasImValue && hasRealValues)
				{
					auto quantityDescriptionComplex(std::make_unique<QuantityDescriptionCurveComplex>());
					valueFormatSetter.setValueFormatRealImaginary(*quantityDescriptionComplex, quantityUnit);

					quantityDescriptionComplex->reserveSizeRealValues(curveData->getYreValues().size());
					for (auto realValue : curveData->getYreValues())
					{
						quantityDescriptionComplex->addValueReal(ot::Variable(realValue));
					}

					quantityDescriptionComplex->reserveSizeImagValues(curveData->getYimValues().size());
					for (auto imValue : curveData->getYimValues())
					{
						quantityDescriptionComplex->addValueImag(ot::Variable(imValue));
					}

					quantityDescription = quantityDescriptionComplex.release();
				}
				else
				{
					auto quantityDescriptionCurve(std::make_unique<QuantityDescriptionCurve>());
					if (hasImValue)
					{
						valueFormatSetter.setValueFormatImaginaryOnly(*quantityDescriptionCurve, quantityUnit);

						quantityDescriptionCurve->reserveDatapointSize(curveData->getYimValues().size());
						for (auto imValue : curveData->getYimValues())
						{
							quantityDescriptionCurve->addDatapoint(ot::Variable(imValue));
						}
					}
					else
					{
						valueFormatSetter.setValueFormatRealOnly(*quantityDescriptionCurve, quantityUnit);

						quantityDescriptionCurve->reserveDatapointSize(curveData->getYreValues().size());
						for (auto reValue : curveData->getYreValues())
						{
							quantityDescriptionCurve->addDatapoint(ot::Variable(reValue));
						}
					}
					quantityDescription = quantityDescriptionCurve.release();
				}

				quantityDescription->setName(quantityName);
				assert(quantityDescription != nullptr);
				newCurveDescription.setQuantityDescription(quantityDescription);
				allCurveDescriptions.push_back(std::move(newCurveDescription));
			}
		}
	}
	return allCurveDescriptions;
}

bool ParametricResult1DManager::extractDataDescriptionSParameter(const std::string& _category, RunIDContainer* _runIDContainer, int _runID, DatasetDescription &dataDescription)
{
	std::map<std::string, Result1DData*> categoryResults = _runIDContainer->getResultsForCategory(_category);
	if (!categoryResults.empty())
	{
		//We determine the characteristics that are shared by all curves.
		auto allParameterDescriptions = extractParameterDescription(_category, _runIDContainer, _runID);

		dataDescription.addParameterDescriptions(allParameterDescriptions);

		//Now we set the s-parameter matrices
		std::vector<Result1DData*> sParameterValues;
		uint32_t numberOfPorts = static_cast<uint32_t>(determineNumberOfPorts(_category, categoryResults, sParameterValues));
		std::string quantityUnit(""), quantityLabel("");
		//parseAxisLabel(curveData->getXLabel(), quantityLabel, quantityUnit);
		std::unique_ptr<QuantityDescriptionSParameter> quantityDescription(std::make_unique<QuantityDescriptionSParameter>(numberOfPorts)); //How to get this information ?
				
		//Dangerous? Do all curves guaranteed have the same number of frequency points ? Are real values always existing ?
		size_t numberOfFrequencyPoints = sParameterValues[0]->getYreValues().size();
		quantityDescription->initiateZeroFilledValueMatrices(numberOfFrequencyPoints);

		quantityDescription->setName("1D Results/S-Parameters");
		ValueFormatSetter valueFormatSetter;
		valueFormatSetter.setValueFormatRealImaginary(*quantityDescription, quantityUnit);

		ot::MatrixEntryPointer matrixEntry;

		for (matrixEntry.m_row= 0; matrixEntry.m_row < numberOfPorts; matrixEntry.m_row++)
		{
			uint32_t index = matrixEntry.m_row * numberOfPorts;
			for (matrixEntry.m_column = 0; matrixEntry.m_column < numberOfPorts; matrixEntry.m_column++)
			{
				if (sParameterValues[index] != nullptr)
				{
					std::vector<double>& realValuesOverAllFrequencies =	sParameterValues[index]->getYreValues();
					std::vector<double>& imValuesOverAllFrequencies =	sParameterValues[index]->getYimValues();
					for (size_t frequencyIndex = 0; frequencyIndex < numberOfFrequencyPoints; frequencyIndex++)
					{
						ot::Variable realValue = ot::Variable(realValuesOverAllFrequencies[frequencyIndex]);
						quantityDescription->setFirstValue(frequencyIndex, matrixEntry,std::move(realValue));
						ot::Variable imagValue = ot::Variable(imValuesOverAllFrequencies[frequencyIndex]);
						quantityDescription->setSecondValue(frequencyIndex, matrixEntry,std::move(imagValue));
					}
				}
				index++;
			}
		}
		dataDescription.setQuantityDescription(quantityDescription.release());
		return true;
	}
	else
	{
		return false;
	}
}

std::list<std::shared_ptr<ParameterDescription>> ParametricResult1DManager::extractParameterDescription(const std::string& _category, RunIDContainer* _runIDContainer, int _runID)
{
	std::list<std::shared_ptr<ParameterDescription>> allParameterDescriptions;
	std::map<std::string, Result1DData*> categoryResults = _runIDContainer->getResultsForCategory(_category);

	//First is the parameter of the X-Axis. Important to stay like this!
	auto firstCurveData = categoryResults.begin()->second;
	std::string xLabel, xUnit;
	parseAxisLabel(firstCurveData->getXLabel(), xLabel, xUnit);
	std::list<ot::Variable> parameterValuesXAxis;
	//parameterValuesXAxis.reserve(firstCurveData->getXValues().size());
	for (const double& xValue : firstCurveData->getXValues())
	{
		parameterValuesXAxis.push_back(ot::Variable(xValue));
	}
	MetadataParameter parameterXAxis;
	parameterXAxis.parameterName = xLabel;
	parameterXAxis.values= std::move(parameterValuesXAxis);
	parameterXAxis.unit = xUnit;
	parameterXAxis.typeName = ot::TypeNames::getDoubleTypeName();
	std::shared_ptr<ParameterDescription> parameterXAxisDescription (std::make_shared<ParameterDescription>(parameterXAxis,false));
	allParameterDescriptions.push_back(std::move(parameterXAxisDescription));

	//Second the run ID
	MetadataParameter parameterRunID;
	parameterRunID.values.push_back(ot::Variable(_runID));
	parameterRunID.parameterName = m_parameterNameRunID;
	parameterRunID.typeName = ot::TypeNames::getInt32TypeName();
	std::shared_ptr<ParameterDescription> parameterDescriptionRunID(std::make_shared<ParameterDescription>(parameterRunID, true));
	allParameterDescriptions.push_back(std::move(parameterDescriptionRunID));

	// Third all structure parameters
	for (auto param : _runIDContainer->getParameters())
	{
		MetadataParameter parameterStructure;
		parameterStructure.parameterName = param.first;
		parameterStructure.values= { ot::Variable(param.second) };
		parameterStructure.typeName = ot::TypeNames::getDoubleTypeName();
		bool isConstantForDataset = parameterStructure.values.size() == 1;
		std::shared_ptr<ParameterDescription> parameterDescriptionStructure(std::make_shared<ParameterDescription>(parameterStructure,isConstantForDataset));
		allParameterDescriptions.push_back(std::move(parameterDescriptionStructure));
	}

	return allParameterDescriptions;
}

//void ParametricResult1DManager::extractMetadataFromDescriptions(std::map<std::string, MetadataQuantity>& _quantitiesByName, std::map<std::string, MetadataParameter>& _parametersByName, std::list<DataDescription1D>& _allDataDescriptions)
//{
//
//	for (auto& dataDescription : _allDataDescriptions)
//	{
//		First, we create a corresponding Quantity 
//		QuantityDescription* quantityDescription = dataDescription.getQuantityDescription();
//		MetadataQuantity&	newMetadataQuantity = quantityDescription->getMetadataQuantity();
//
//		auto quantityMetadataByName = _quantitiesByName.find(newMetadataQuantity.quantityName);
//		if (quantityMetadataByName != _quantitiesByName.end())
//		{
//			The Quantity was already added. So we need to check if the information about this quantity are consistent.
//			MetadataQuantity& existingQuantityMetadata = quantityMetadataByName->second;
//			if (newMetadataQuantity == existingQuantityMetadata)
//			{
//				Quantity exists already by name, but holds different characteristics
//				Display: Warning regarding changed name.
//				newMetadataQuantity.quantityLabel = newMetadataQuantity.quantityName;
//				int i = 0;
//				do
//				{
//					i++;
//					newMetadataQuantity.quantityName = newMetadataQuantity.quantityName + "_" + std::to_string(i);
//					quantityMetadataByName = _quantitiesByName.find(newMetadataQuantity.quantityName);
//				} while (quantityMetadataByName != _quantitiesByName.end());
//
//				Name eventuell mit _ erweitern
//				Originalen Namen behalten, ansonsten 
//				dataDescription.quantityName = newInternalName
//				Einmal einen UniqueName und dann ein Label. UniqueName = Label, außer UniqueName existiert bereits. Dann wird nämlich hochgezählt
//				Das löst aber nicht das Problem bei Verteilung. Es kann sein, dass zwei Datensätze parallel hinzugefügt werden. Die UID unterscheidet sich. Der Unique Name ist aber identisch.
//				_quantitiesByName[newMetadataQuantity.quantityName] = newMetadataQuantity;
//			}
//		}
//		else
//		{
//			_quantitiesByName[newMetadataQuantity.quantityName] = newMetadataQuantity;
//		}
//
//		Now we create the parameter
//		addParameterMetadata(_parametersByName, dataDescription.getXAxisParameter());
//
//		for (auto& parameter : dataDescription.getParameters())
//		{
//			addParameterMetadata(_parametersByName, parameter);
//		}
//
//	}
//}
//
//void ParametricResult1DManager::addParameterMetadata(std::map<std::string, MetadataParameter>& _parametersByName, std::shared_ptr<MetadataParameter> _parameter)
//{
//	auto parameterByName = _parametersByName.find(_parameter->parameterName);
//	if (parameterByName != _parametersByName.end())
//	{
//		auto existingParameter = parameterByName->second;
//		if (existingParameter == *_parameter.get())
//		{
//
//			Needs a notification
//			There is already a parameter with the same name but differences in its other criterias.
//			_parameter->parameterLabel = _parameter->parameterName;
//			int i = 0;
//			do
//			{
//				i++;
//				_parameter->parameterName = _parameter->parameterName + "_" + std::to_string(i);
//				parameterByName = _parametersByName.find(_parameter->parameterName);
//			} while (parameterByName != _parametersByName.end());
//
//			_parametersByName[_parameter->parameterName] = *_parameter.get();
//		}
//		else
//		{
//
//			std::list<ot::Variable>* existingValues = &(existingParameter.values);
//			std::list<ot::Variable> newValues = { _parameter->values.begin(), _parameter->values.end() };
//			existingValues->insert(existingValues->end(), newValues.begin(), newValues.end());
//			existingValues->sort();
//			existingValues->unique();
//		}
//	}
//	else
//	{
//		_parametersByName[_parameter->parameterName] = *_parameter.get();
//	}
//}

//MetadataQuantity ParametricResult1DManager::createNewQuantity(const QuantityDescription* const _quantityDescription)
//{
//	MetadataQuantity newQuantity;
//	newQuantity.dataColumns = _quantityDescription->quantityDataNumberOfColumns;
//	newQuantity.dataRows = _quantityDescription->quantityDataNumberOfRows;
//	newQuantity.quantityName = _quantityDescription->quantityName;
//	newQuantity.quantityLabel = _quantityDescription->quantityLabel;
//	newQuantity.unit = _quantityDescription->quantityUnit;
//	newQuantity.typeName = ot::TypeNames::getDoubleTypeName();
//	return newQuantity;
//}
//
//MetadataParameter ParametricResult1DManager::createNewParameter(const ParameterDescription* const _paramParameter)
//{
//	MetadataParameter newParameterMetadata;
//	newParameterMetadata.parameterName = _paramParameter->parameterName;
//	newParameterMetadata.parameterLabel = _paramParameter->parameterLabel;
//	newParameterMetadata.unit = _paramParameter->unitName;
//	newParameterMetadata.typeName = _paramParameter->valueDataType;
//	newParameterMetadata.values = { _paramParameter->parameterValues.begin(),_paramParameter->parameterValues.end() };
//	newParameterMetadata.values.sort();
//	newParameterMetadata.values.unique();
//	return newParameterMetadata;
//}

int ParametricResult1DManager::determineNumberOfPorts(const std::string& category, std::map<std::string, Result1DData*>& categoryResults, std::vector<Result1DData*>& sources)
{
	int maxPort = 0;

	for (auto item : categoryResults)
	{
		std::string itemName = item.first.substr(category.size() + 2); // We skip the path and the leading S in front of the port identifier i,j

		size_t index = itemName.find(',');
		assert(index != std::string::npos);

		std::string port1 = itemName.substr(0, index);
		std::string port2 = itemName.substr(index + 1);

		int nPort1 = atoi(port1.c_str());
		int nPort2 = atoi(port2.c_str());

		maxPort = std::max(maxPort, nPort1);
		maxPort = std::max(maxPort, nPort2);
	}

	sources.resize(maxPort * maxPort);

	for (auto item : categoryResults)
	{
		std::string itemName = item.first.substr(category.size() + 2); // We skip the path and the leading S in front of the port identifier i,j

		size_t index = itemName.find(',');
		assert(index != std::string::npos);

		std::string port1 = itemName.substr(0, index);
		std::string port2 = itemName.substr(index + 1);

		int nPort1 = atoi(port1.c_str());
		int nPort2 = atoi(port2.c_str());

		sources[(nPort1 - 1) * maxPort + (nPort2 - 1)] = item.second;
	}

	return maxPort;
}

void ParametricResult1DManager::parseAxisLabel(const std::string& value, std::string& label, std::string& unit)
{
	// Here we separate the value into the actual axis type and the unit. We assume that both are separated by a / character
	size_t separatorIndex = value.find('/');

	label = value.substr(0, separatorIndex);
	unit = value.substr(separatorIndex + 1);

	boost::trim(label);
	boost::trim(unit);
}

