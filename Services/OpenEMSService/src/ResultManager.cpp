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

#include "Application.h"
#include "ResultManager.h"

#include "ParametricCombination.h"

#include "OTResultDataAccess/CurveFactory.h"
#include "OTCore/MetadataHandle/MetadataSeries.h"
#include "OTCore/MetadataHandle/MetadataParameter.h"

#include "OTCore/MetadataEntry/MetadataEntrySingle.h"
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescription.h"
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionCurve.h"
#include "OTResultDataAccess/ResultCollection/ResultCollectionExtender.h"
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionCurveComplex.h"

#include "OTCore/FolderNames.h"
#include "OTGui/Painter/PainterRainbowIterator.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include "OTModelEntities/EntityFile.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityResult1DPlot.h"

#include <regex>
#include <filesystem>
#include <set>
#include <complex>
#include <fstream>

ResultManager::ResultManager(ot::components::ModelComponent* modelComponent, const std::string& _tmpDirPath, const std::string& _resultBasePath)
{
	setModelComponent(modelComponent);
	tmpDirPath = _tmpDirPath;
	resultBasePath = _resultBasePath;

	clear();
	loadParameters();
}

ResultManager::~ResultManager()
{

}

void ResultManager::clear()
{
	// We delete all previous result data (series), since we will recreate it if necessary
	std::list<std::string> resultEntity{ ot::FolderNames::DatasetFolder + "/OpenEMS Results", resultBasePath + "/1D Results" };
	ot::ModelServiceAPI::deleteEntitiesFromModel(resultEntity, false);
}

void ResultManager::loadParameters()
{
	// TODO: Process the parameters and store them in the ParametricCombination::parameters

}

void ResultManager::convert1D(const std::string& resultName, const std::string& fileName, const std::string& quantityName, const std::string& quantityUnit, const std::string& xAxis, const std::string& xUnit, double xScale)
{
	std::vector<std::pair<double, std::complex<double>>> curveData;
	bool isComplex = false;

	if (load1Ddata(tmpDirPath + "/" + fileName, curveData, isComplex, xScale))
	{
		std::list<ot::Variable> parameterValuesXAxis;
		
		for (size_t index = 0; index < curveData.size(); index++)
		{
			parameterValuesXAxis.push_back(ot::Variable(curveData[index].first));
		}
		
		std::list<std::shared_ptr<ParameterDescription>> allParameterDescriptions;
		
		// Add the parameter descriptions for the x-axis and optionally all other parameters
		addParameterDescriptions(parameters, xAxis, xUnit, parameterValuesXAxis, allParameterDescriptions);
		
		// Add the data of all result curves for the current parameters.
		addCurveData(resultName, quantityName, quantityUnit, curveData, isComplex, allParameterDescriptions, allCurveDescriptions);

		// Add the corresponding plot entry
		plotContainers[getPlotName(resultName)] = xAxis;
	}
}

void ResultManager::storeResults()
{
	storeCurves(allCurveDescriptions);
}

std::string ResultManager::getPlotName(const std::string& resultName)
{
	size_t pos = resultName.find_last_of('/');
	if (pos == std::string::npos)
	{
		assert(0);
		return "";
	}

	return resultName.substr(0, pos);
}


bool ResultManager::load1Ddata(const std::string filePath, std::vector<std::pair<double, std::complex<double>>>& curveData, bool& isComplex, double xScale)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		return false;
	}

	curveData.clear();
	isComplex = false;

	std::string line;
	while (std::getline(file, line))
	{
		// Skip empty lines
		if (line.empty())
		{
			continue;
		}

		std::istringstream iss(line);

		double x;
		double realPart;
		double imagPart = 0.0;

		// The first two values are mandatory
		if (!(iss >> x >> realPart))
		{
			return false;
		}

		// Third value is optional (imaginary part)
		if (iss >> imagPart)
		{
			isComplex = true;
		}

		// Check for unexpected additional values
		double extra;
		if (iss >> extra)
		{
			return false;
		}

		// Store the parsed values
		curveData.emplace_back(
			x / xScale,
			std::complex<double>(realPart, imagPart));
	}

	return true;
}

void ResultManager::addParameterDescriptions(ParametricCombination &currentRun, const std::string &xAxis, const std::string &xUnit, std::list<ot::Variable> parameterValuesXAxis, std::list<std::shared_ptr<ParameterDescription>> &allParameterDescriptions)
{
	// First add the parameter for the x-axis
	MetadataParameter parameterXAxis;
	parameterXAxis.parameterName = xAxis;
	parameterXAxis.values = std::move(parameterValuesXAxis);
	parameterXAxis.unit = xUnit;
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

void ResultManager::addCurveData(const std::string& resultName, const std::string &quantityName, const std::string &quantityUnit, std::vector<std::pair<double, std::complex<double>>>& curveData, bool& isComplex, std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions, std::list<DatasetDescription>& allCurveDescriptions)
{
	DatasetDescription newCurveDescription;
	newCurveDescription.addParameterDescriptions(allParameterDescriptions);

	QuantityDescription* quantityDescription = nullptr;

	//Values are either complex or real
	if (isComplex)
	{
		auto quantityDescriptionComplex(std::make_unique<QuantityDescriptionCurveComplex>());
		quantityDescriptionComplex->defineQuantityAsComplex(ot::ComplexNumberFormat::Cartesian, ot::TypeNames::getDoubleTypeName(), quantityUnit, quantityUnit);
		
		quantityDescriptionComplex->reserveSizeRealValues(curveData.size());
		for (size_t index = 0; index < curveData.size(); index++)
		{
			quantityDescriptionComplex->addValueReal(ot::Variable(curveData[index].second.real()));
		}

		quantityDescriptionComplex->reserveSizeImagValues(curveData.size());
		for (size_t index = 0; index < curveData.size(); index++)
		{
			quantityDescriptionComplex->addValueImag(ot::Variable(curveData[index].second.imag()));
		}

		quantityDescription = quantityDescriptionComplex.release();
	}
	else
	{
		auto quantityDescriptionCurve(std::make_unique<QuantityDescriptionCurve>());
		quantityDescriptionCurve->defineQuantityAsSingle(ot::TypeNames::getDoubleTypeName(), quantityUnit);
			
		quantityDescriptionCurve->reserveDatapointSize(curveData.size());
		for (size_t index = 0; index < curveData.size(); index++)
		{
			quantityDescriptionCurve->addDatapoint(ot::Variable(curveData[index].second.real()));
		}

		quantityDescription = quantityDescriptionCurve.release();
	}
	assert(quantityDescription != nullptr);

	quantityDescription->setName(resultName);
	newCurveDescription.setQuantityDescription(quantityDescription);
	allCurveDescriptions.push_back(std::move(newCurveDescription));
}

void ResultManager::storeCurves(std::list<DatasetDescription> &allCurveDescriptions)
{
	//First we access the result collection of the current project
	std::string collectionName = DataBase::instance().getCollectionName();

	ResultCollectionExtender resultCollectionExtender(collectionName, *_modelComponent);
	resultCollectionExtender.registerCallbacks(
		ot::EntityCallbackBase::Callback::Properties |
		ot::EntityCallbackBase::Callback::Selection |
		ot::EntityCallbackBase::Callback::DataNotify,
		Application::instance()->getServiceName()
	);
	resultCollectionExtender.setSaveModel(false);

	std::string seriesName = createNewUniqueTopologyName(ot::FolderNames::DatasetFolder, "OpenEMS Results");

	uint64_t seriesMetadataIndex = resultCollectionExtender.buildSeriesMetadata(allCurveDescriptions, seriesName);
	resultCollectionExtender.storeCampaignChanges();

	const MetadataSeries* series = resultCollectionExtender.findMetadataSeries(seriesMetadataIndex);

	// First, we create new plots
	std::map<std::string, ot::PainterRainbowIterator*> rainbowIterators;

	for (auto plot : plotContainers)
	{
		EntityResult1DPlot newPlot(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
		newPlot.setName(resultBasePath + "/1D Results/" + plot.first);
		newPlot.createProperties();

		std::string defaultAxis = plot.second;
		ot::Plot1DCfg plotCfg;
		plotCfg.setTitle("Title");
		plotCfg.setXAxisParameter(defaultAxis);
		newPlot.setPlot(plotCfg);
		newPlot.storeToDataBase();
		_modelComponent->addNewTopologyEntity(newPlot.getEntityID(), newPlot.getEntityStorageVersion(), false);

		rainbowIterators[plot.first] = new ot::PainterRainbowIterator;
	}

	//Now we store all data points in the result collection
	std::set<std::string> createdCurves;

	for (DatasetDescription& dataDescription : allCurveDescriptions)
	{
		try
		{
			resultCollectionExtender.processDataPoints(&dataDescription, seriesMetadataIndex);

			// Add the curve to the plots
			std::string fullName = dataDescription.getQuantityDescription()->getName();

			std::string plotName = getPlotName(fullName);
			ot::PainterRainbowIterator* plotPainter = rainbowIterators[plotName];

			if (!plotName.empty())
			{
				std::string curveName = resultBasePath + "/1D Results/" + fullName;

				ot::Plot1DCurveCfg curveConfig;

				auto painter = plotPainter->getNextPainter();
				curveConfig.setLinePenPainter(painter.release());
				curveConfig.setLinePenWidth(2.0);

				CurveFactory::addToConfig(*series, dataDescription.getQuantityDescription()->getMetadataQuantity(),curveConfig, Application::instance(), &resultCollectionExtender);

				EntityResult1DCurve newCurve(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
				newCurve.setName(curveName);
				newCurve.createProperties();
				newCurve.setStaticCurveQueryOptions(curveConfig);
				newCurve.storeToDataBase();

				_modelComponent->addNewTopologyEntity(newCurve.getEntityID(), newCurve.getEntityStorageVersion(), false);
			}
		}
		catch (std::exception e)
		{
			OT_LOG_E("Unable to import data: " + dataDescription.getQuantityDescription()->getName() + " (" + e.what() + ")");
		}
	}
}

