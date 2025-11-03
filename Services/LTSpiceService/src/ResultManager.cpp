// @otlicense
// File: ResultManager.cpp
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

#include "ResultManager.h"

#include "ParametricCombination.h"
#include "LTSpiceRawReader.h"

#include "ResultCollectionExtender.h"
#include "MetadataSeries.h"
#include "MetadataParameter.h"
#include "MetadataEntrySingle.h"
#include "ValueFormatSetter.h"

#include "QuantityDescription.h"
#include "QuantityDescriptionCurve.h"
#include "QuantityDescriptionCurveComplex.h"

#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCore/FolderNames.h"

#include "EntityFile.h"
#include "EntityAPI.h"
#include "DataBase.h"

#include <regex>
#include <filesystem>

ResultManager::ResultManager(ot::components::ModelComponent* modelComponent)
{
	setModelComponent(modelComponent);
}

ResultManager::~ResultManager()
{

}

void ResultManager::getParametricCombinations(const std::string& logFileName, std::list<ParametricCombination>& parameterRuns)
{
	parameterRuns.clear();

	// Read the data of the log file entity into a string logFileContent
	ot::EntityInformation entityInfo;
	ot::ModelServiceAPI::getEntityInformation(logFileName, entityInfo);

	EntityFile* fileEntity = dynamic_cast<EntityFile*>(ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion()));
	if (fileEntity == nullptr) return;

	std::shared_ptr<EntityBinaryData> data = fileEntity->getDataEntity();
	if (data == nullptr) return;

	std::vector<char> logFileData = data->getData();
	std::string logFileContent(logFileData.begin(), logFileData.end());

	// Now extract all lines with a step message from the logfile data
	std::vector<std::string> stepLines;
	std::istringstream stream(logFileContent);
	std::string line;

	while (std::getline(stream, line)) {
		std::string lowerStr = line;
		std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
		if (lowerStr.find(".step") != std::string::npos) {
			stepLines.push_back(line);
		}
	}

	// Finally, we need to process each line and extract its parameter combinations
	std::vector<std::pair<std::string, double>> params;
	std::regex stepRegex(R"((\w+)=([-+]?\d*\.?\d+))"); // Regex for Parameter=Value
	std::smatch match;

	for (auto line : stepLines)
	{
		ParametricCombination parameterCombination;

		auto searchStart = line.cbegin();
		while (std::regex_search(searchStart, line.cend(), match, stepRegex)) {
			std::string param = match[1].str();           // parameter name
			double value = std::stod(match[2].str());     // parameter value
			parameterCombination.addParameter(param, value);
			searchStart = match.suffix().first;
		}

		parameterRuns.push_back(parameterCombination);
	}
}

void ResultManager::extractResults(const std::string &ltSpicefileNameBase)
{
	clear();

	std::string projectFileName = "Files/" + std::filesystem::path(ltSpicefileNameBase).stem().string();
	if (projectFileName.empty()) return;

	// First, we get a list of all parametric runs and their parameter combinations from the .log file
	std::list<ParametricCombination> parameterRuns;
	getParametricCombinations(projectFileName + ".log", parameterRuns);

	// Now we read the data of the .raw file
	std::string rawFileName = projectFileName + ".raw";
	ot::EntityInformation entityInfo;
	ot::ModelServiceAPI::getEntityInformation(rawFileName, entityInfo);

	EntityFile* fileEntity = dynamic_cast<EntityFile*>(ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion()));
	if (fileEntity == nullptr) return;

	std::shared_ptr<EntityBinaryData> data = fileEntity->getDataEntity();
	if (data == nullptr) return;

	std::vector<char> rawFileData = data->getData();

	// Read the curve data from the raw file data
	ltspice::AutoRawReader reader;
	ltspice::RawData resultData = reader.readFromBytes(std::string(rawFileData.data(), rawFileData.size()));

	// Convert the raw result data into curves
	std::list<DatasetDescription> allCurveDescriptions;
	processParametricResults(resultData, parameterRuns, allCurveDescriptions);

	// Finally store the curves in the result data base
	storeCurves(allCurveDescriptions);
}

void ResultManager::clear()
{
	// We delete all previous result data (series), since we will recreate it if necessary
	std::list<std::string> resultEntity{ ot::FolderNames::DatasetFolder + "/LTSpice Imported Results" };
	ot::ModelServiceAPI::deleteEntitiesFromModel(resultEntity, false);
}

void ResultManager::processParametricResults(ltspice::RawData& resultData, std::list<ParametricCombination>& parameterRuns, std::list<DatasetDescription> &allCurveDescriptions)
{
	if (parameterRuns.empty())
	{
		// If we have no parametric runs, we add a dummy run 
		ParametricCombination parameterCombination;
		parameterRuns.push_back(parameterCombination);
	}

	size_t numberOfParameterRuns = parameterRuns.size();
	size_t numberOfXValues       = resultData.points() / numberOfParameterRuns;

	size_t indexOffset = 0;

	// Iterate over all parametric combinations. The results are then stored for each parameter combination individually
	for (auto currentRun : parameterRuns)
	{
		std::list<ot::Variable> parameterValuesXAxis;

		if (resultData.isComplex())
		{
			for (size_t index = 0; index < numberOfXValues; index++)
			{
				parameterValuesXAxis.push_back(ot::Variable(resultData.complex(index + indexOffset, 0).real()));
			}
		}
		else
		{
			for (size_t index = 0; index < numberOfXValues; index++)
			{
				parameterValuesXAxis.push_back(ot::Variable(resultData.real(index + indexOffset, 0)));
			}
		}

		std::list<std::shared_ptr<ParameterDescription>> allParameterDescriptions;

		// Add the parameter descriptions for the x-axis and optionally all other parameters
		addParameterDescriptions(currentRun, resultData, parameterValuesXAxis, allParameterDescriptions);

		// Add the data of all result curves for the current parameters.
		addCurveData(resultData, allParameterDescriptions, allCurveDescriptions, numberOfXValues, indexOffset);

		indexOffset += numberOfXValues;
	}
}

void ResultManager::addParameterDescriptions(ParametricCombination &currentRun, ltspice::RawData& resultData, std::list<ot::Variable> parameterValuesXAxis, std::list<std::shared_ptr<ParameterDescription>> &allParameterDescriptions)
{
	// First add the parameter for the x-axis
	MetadataParameter parameterXAxis;
	parameterXAxis.parameterName = resultData.variables()[0].name();
	parameterXAxis.values = std::move(parameterValuesXAxis);
	parameterXAxis.unit = getUnitFromType(resultData.variables()[0].type());
	parameterXAxis.typeName = ot::TypeNames::getDoubleTypeName();
	std::shared_ptr<ParameterDescription> parameterXAxisDescription(std::make_shared<ParameterDescription>(parameterXAxis, false));
	allParameterDescriptions.push_back(std::move(parameterXAxisDescription));

	// Now add the values of all sweep parameters
	for (auto parameter : currentRun.getParameters())
	{
		MetadataParameter parameterStructure;
		parameterStructure.parameterName = parameter.first;
		parameterStructure.values = { ot::Variable(parameter.second) };
		parameterStructure.typeName = ot::TypeNames::getDoubleTypeName();
		bool isConstantForDataset = parameterStructure.values.size() == 1;
		std::shared_ptr<ParameterDescription> parameterDescriptionStructure(std::make_shared<ParameterDescription>(parameterStructure, isConstantForDataset));
		allParameterDescriptions.push_back(std::move(parameterDescriptionStructure));
	}
}

std::string ResultManager::getUnitFromType(const std::string& type)
{
	if (type == "time")
	{
		return "s";
	}
	else if (type == "frequency")
	{
		return "Hz";
	}
	else if (type == "voltage")
	{
		return "V";
	}
	else if (type == "device_current")
	{
		return "A";
	}
	else
	{
		assert(0); // Unknown type
	}
	return "";
}

void ResultManager::addCurveData(ltspice::RawData& resultData, std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions, std::list<DatasetDescription>& allCurveDescriptions, size_t numberOfXValues, size_t indexOffset)
{
	for (size_t iVariable = 1; iVariable < resultData.variablesCount(); iVariable++)
	{
		DatasetDescription newCurveDescription;
		newCurveDescription.addParameterDescriptions(allParameterDescriptions);

		std::string quantityUnit = getUnitFromType(resultData.variables()[iVariable].type());
		std::string quantityName = resultData.variables()[iVariable].name();;

		QuantityDescription* quantityDescription = nullptr;
		ValueFormatSetter valueFormatSetter;

		//Values are either complex or real
		if (resultData.isComplex())
		{
			auto quantityDescriptionComplex(std::make_unique<QuantityDescriptionCurveComplex>());
			valueFormatSetter.setValueFormatRealImaginary(*quantityDescriptionComplex, quantityUnit);

			quantityDescriptionComplex->reserveSizeRealValues(numberOfXValues);
			for (size_t index = 0; index < numberOfXValues; index++)
			{
				quantityDescriptionComplex->addValueReal(ot::Variable(resultData.complex(index + indexOffset, iVariable).real()));
			}

			quantityDescriptionComplex->reserveSizeImagValues(numberOfXValues);
			for (size_t index = 0; index < numberOfXValues; index++)
			{
				quantityDescriptionComplex->addValueImag(ot::Variable(resultData.complex(index + indexOffset, iVariable).imag()));
			}

			quantityDescription = quantityDescriptionComplex.release();
		}
		else
		{
			auto quantityDescriptionCurve(std::make_unique<QuantityDescriptionCurve>());
			valueFormatSetter.setValueFormatRealOnly(*quantityDescriptionCurve, quantityUnit);

			quantityDescriptionCurve->reserveDatapointSize(numberOfXValues);
			for (size_t index = 0; index < numberOfXValues; index++)
			{
				quantityDescriptionCurve->addDatapoint(ot::Variable(resultData.real(index + indexOffset, iVariable)));
			}

			quantityDescription = quantityDescriptionCurve.release();
		}
		assert(quantityDescription != nullptr);

		quantityDescription->setName(quantityName);
		newCurveDescription.setQuantityDescription(quantityDescription);
		allCurveDescriptions.push_back(std::move(newCurveDescription));
	}
}

void ResultManager::storeCurves(std::list<DatasetDescription> &allCurveDescriptions)
{
	//First we access the result collection of the current project
	std::string collectionName = DataBase::instance().getCollectionName();

	ResultCollectionExtender resultCollectionExtender(collectionName, *_modelComponent, OT_INFO_SERVICE_TYPE_LTSPICE);
	resultCollectionExtender.setSaveModel(false);

	std::string seriesName = CreateNewUniqueTopologyName(ot::FolderNames::DatasetFolder, "LTSpice Imported Results");

	std::list<std::shared_ptr<MetadataEntry>> seriesMetadata;
	uint64_t seriesMetadataIndex = resultCollectionExtender.buildSeriesMetadata(allCurveDescriptions, seriesName, seriesMetadata);
	resultCollectionExtender.storeCampaignChanges();
	//Now we store all data points in the result collection
	for (DatasetDescription& dataDescription : allCurveDescriptions)
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

