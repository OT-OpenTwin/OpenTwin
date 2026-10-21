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
#include "FarfieldReader.h"

#include "OTResultDataAccess/CurveFactory.h"
#include "OTCore/MetadataHandle/MetadataSeries.h"
#include "OTCore/MetadataHandle/MetadataParameter.h"
#include "OTCore/Tuple/TupleDescriptionSingle.h"

#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescription.h"
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionCurve.h"
#include "OTResultDataAccess/ResultCollection/ResultCollectionExtender.h"

#include "OTCore/FolderNames.h"
#include "OTGui/Painter/PainterRainbowIterator.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include "OTModelEntities/EntityFile.h"
#include "OTModelEntities/EntityParameter.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityResult1DPlot.h"
#include "OTModelEntities/EntityResultFarfield.h"

#include "OTCADEntities/EntityFarfieldDump.h"

#include <regex>
#include <filesystem>
#include <set>
#include <complex>
#include <fstream>

ResultManager::ResultManager(ot::components::ModelComponent* modelComponent, const std::string& _tmpDirPath, const std::string &_resultSeriesName, const std::string& _resultBasePath)
{
	setModelComponent(modelComponent);
	tmpDirPath = _tmpDirPath;
	resultBasePath = _resultBasePath;
	resultSeriesName = _resultSeriesName;

	clear();
	loadParameters();
}

ResultManager::~ResultManager()
{

}

void ResultManager::clear()
{
	// We delete all previous result data (series), since we will recreate it if necessary
	std::list<std::string> resultEntity{ ot::FolderNames::DatasetFolder + "/" + resultSeriesName, resultBasePath + "/1D Results" };
	ot::ModelServiceAPI::deleteEntitiesFromModel(resultEntity, false);
}

void ResultManager::loadParameters()
{
	ot::UIDList parameterList = ot::ModelServiceAPI::getIDsOfFolderItemsOfType("Parameters", "EntityParameter", true);

	std::list<ot::EntityInformation> parameterInformation;
	ot::ModelServiceAPI::getEntityInformation(parameterList, parameterInformation);

	DataBase::instance().prefetchDocumentsFromStorage(parameterInformation);

	for (auto param : parameterInformation)
	{
		EntityParameter* entityParam = dynamic_cast<EntityParameter*>(ot::EntityAPI::readEntityFromEntityIDandVersion(param.getEntityID(), param.getEntityVersion()));

		if (entityParam != nullptr)
		{
			std::string parameterName = entityParam->getName().substr(std::string("Parameters/").size());
			parameters.addParameter(parameterName, entityParam->getNumericValue());
			
			delete entityParam;
			entityParam = nullptr;
		}
	}
}

void ResultManager::convert1D(const std::string& resultName, const std::string& fileName, const std::string& quantityName, const std::string& quantityUnit, const std::string& xAxis, const std::string& xUnit, double xScale, int readDataColumnOnly)
{
	// The optional parameter readDataColumnOnly determines whether only a single result column should be read in case that multiple columns are present in the file.
	// By default, the parameter is set to -1 which means that all columns are read.
	// If the parameter is set to a value larger or equal to 1, only the corresponding column will be read (0 corresponds to the x axis).
	assert(readDataColumnOnly == -1 || readDataColumnOnly >= 1);

	std::vector<std::pair<double, std::complex<double>>> curveData;
	bool isComplex = false;

	if (load1Ddata(tmpDirPath + "/" + fileName, curveData, isComplex, xScale, readDataColumnOnly))
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
		addCurveData(resultName, quantityName, quantityUnit, curveData, isComplex, allParameterDescriptions);

		// Add the corresponding plot entry
		plotContainers[getPlotName(resultName)] = xAxis;
	}
}

void ResultManager::convertFarfield(const std::string& fileName, const std::string &frequencyUnit, double frequencyScale, const std::string& excitationString)
{
	std::vector<double> frequency;
	std::vector<double> theta;
	std::vector<double> phi;
	std::vector<double> eAbs;

	std::vector<std::complex<double>> eTheta;
	std::vector<std::complex<double>> ePhi;

	if (readFarFieldTable(fileName, frequency, theta, phi, eTheta, ePhi, eAbs))
	{
		std::list<std::shared_ptr<ParameterDescription>> allParameterDescriptions;

		addFarfieldParameterDescriptions(parameters, frequencyUnit, frequencyScale, frequency, theta, phi, allParameterDescriptions);

		addFarfieldData(excitationString, eTheta, ePhi, eAbs, allParameterDescriptions);
	}
}

void ResultManager::storeResults(const std::list<std::string> &excitationStringList, const std::list<EntityFarfieldDump*>& farfieldDumpList)
{
	storeCurves(excitationStringList, farfieldDumpList);
}

std::string ResultManager::getPlotName(const std::string& resultName)
{
	size_t pos = resultName.find_last_of('/');
	if (pos == std::string::npos)
	{
		return "";
	}

	return resultName.substr(0, pos);
}


bool ResultManager::load1Ddata(const std::string filePath, std::vector<std::pair<double, std::complex<double>>>& curveData, bool& isComplex, double xScale, int readDataColumnOnly)
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

		// Skip comment lines
		if (line[0] == '%')
		{
			continue;
		}

		std::istringstream iss(line);

		double x;
		double realPart = 0.0;
		double imagPart = 0.0;

		// The first two value is mandatory
		if (!(iss >> x))
		{
			return false;
		}

		if (readDataColumnOnly > 0)
		{
			// Now we skip all columns before the requested one
			for (int i = 1; i < readDataColumnOnly; i++)
			{
				double dummy = 0.0;
				if (!(iss >> dummy))
				{
					return false;
				}
			}
		}

		// Read the first data column
		if (!(iss >> realPart))
		{
			return false;
		}

		if (readDataColumnOnly == -1)
		{
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

void ResultManager::addFarfieldParameterDescriptions(ParametricCombination& currentRun, const std::string &frequencyUnit, double frequencyScale, std::vector<double>& frequency, std::vector<double>& theta, std::vector<double>& phi, std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions)
{
	std::list<ot::Variable> frequencyList, thetaList, phiList;

	for (auto frequencyValue : frequency) frequencyList.push_back(ot::Variable(frequencyValue / frequencyScale));
	for (auto thetaValue : theta) thetaList.push_back(ot::Variable(thetaValue));
	for (auto phiValue : phi) phiList.push_back(ot::Variable(phiValue));

	// Add frequency
	MetadataParameter parameterFrequency;
	parameterFrequency.parameterName = "Frequency";
	parameterFrequency.values = std::move(frequencyList);
	parameterFrequency.unit = frequencyUnit;
	parameterFrequency.typeName = ot::TypeNames::getDoubleTypeName();
	std::shared_ptr<ParameterDescription> parameterDescriptionFrequency(std::make_shared<ParameterDescription>(parameterFrequency, false));
	allParameterDescriptions.push_back(std::move(parameterDescriptionFrequency));

	// Add theta
	MetadataParameter parameterTheta;
	parameterTheta.parameterName = "Theta";
	parameterTheta.values = std::move(thetaList);
	parameterTheta.unit = "degrees";
	parameterTheta.typeName = ot::TypeNames::getDoubleTypeName();
	std::shared_ptr<ParameterDescription> parameterDescriptionTheta(std::make_shared<ParameterDescription>(parameterTheta, false));
	allParameterDescriptions.push_back(std::move(parameterDescriptionTheta));

	// First add the parameter for the x-axis (Phi)
	MetadataParameter parameterPhi;
	parameterPhi.parameterName = "Phi";
	parameterPhi.values = std::move(phiList);
	parameterPhi.unit = "degrees";
	parameterPhi.typeName = ot::TypeNames::getDoubleTypeName();
	std::shared_ptr<ParameterDescription> parameterPhiDescription(std::make_shared<ParameterDescription>(parameterPhi, false));
	allParameterDescriptions.push_back(std::move(parameterPhiDescription));

	// Now add the values of all sweep parameters
	for (auto parameter : currentRun.getParameters())
	{
		MetadataParameter parameterStructure;
		parameterStructure.parameterName = parameter.first;
		parameterStructure.values = { ot::Variable(parameter.second) };
		parameterStructure.typeName = ot::TypeNames::getDoubleTypeName();
		std::shared_ptr<ParameterDescription> parameterDescriptionStructure(std::make_shared<ParameterDescription>(parameterStructure, true));
		allParameterDescriptions.push_back(std::move(parameterDescriptionStructure));
	}
}

void ResultManager::addCurveData(const std::string& resultName, const std::string &quantityName, const std::string &quantityUnit, std::vector<std::pair<double, std::complex<double>>>& curveData, bool& isComplex, std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions)
{
	DatasetDescription newCurveDescription;
	newCurveDescription.addParameterDescriptions(allParameterDescriptions);

	QuantityDescription* quantityDescription = nullptr;

	//Values are either complex or real
	if (isComplex)
	{
		auto quantityDescriptionComplex(std::make_unique<QuantityDescriptionCurve>());
		quantityDescriptionComplex->defineQuantityAsComplex(ot::ComplexNumberFormat::Cartesian, ot::TypeNames::getDoubleTypeName(), quantityUnit, quantityUnit);
		
		quantityDescriptionComplex->reserveDatapointSize(curveData.size());
		for (size_t index = 0; index < curveData.size(); index++)
		{
			ot::Variable entry (curveData[index].second);
			quantityDescriptionComplex->addDatapoint(std::move(entry));
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

void ResultManager::addFarfieldData(const std::string& excitationString, std::vector<std::complex<double>> &eTheta, std::vector<std::complex<double>> &ePhi, std::vector<double> &eAbs,
									std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions)
{
	// Add Etheta quantity
	{
		DatasetDescription newCurveDescription;
		newCurveDescription.addParameterDescriptions(allParameterDescriptions);

		auto quantityDescriptionComplex(std::make_unique<QuantityDescriptionCurve>());
		quantityDescriptionComplex->defineQuantityAsComplex(ot::ComplexNumberFormat::Cartesian, ot::TypeNames::getDoubleTypeName(), "", "");
		quantityDescriptionComplex->reserveDatapointSize(eTheta.size());
		for (size_t index = 0; index < eTheta.size(); index++)
		{
			ot::Variable entry(eTheta[index]);
			quantityDescriptionComplex->addDatapoint(std::move(entry));
		}

		QuantityDescription *quantityDescription = quantityDescriptionComplex.release();
		quantityDescription->setName("Farfields/E_Theta" + excitationString);
		newCurveDescription.setQuantityDescription(quantityDescription);
		allCurveDescriptions.push_back(std::move(newCurveDescription));
	}

	// Add Ephi quantity
	{
		DatasetDescription newCurveDescription;
		newCurveDescription.addParameterDescriptions(allParameterDescriptions);

		auto quantityDescriptionComplex(std::make_unique<QuantityDescriptionCurve>());
		quantityDescriptionComplex->defineQuantityAsComplex(ot::ComplexNumberFormat::Cartesian, ot::TypeNames::getDoubleTypeName(), "", "");
		quantityDescriptionComplex->reserveDatapointSize(ePhi.size());
		for (size_t index = 0; index < ePhi.size(); index++)
		{
			ot::Variable entry(ePhi[index]);
			quantityDescriptionComplex->addDatapoint(std::move(entry));
		}

		QuantityDescription* quantityDescription = quantityDescriptionComplex.release();
		quantityDescription->setName("Farfields/E_Phi" + excitationString);
		newCurveDescription.setQuantityDescription(quantityDescription);
		allCurveDescriptions.push_back(std::move(newCurveDescription));
	}

	// Add Eabs quantity
	{
		DatasetDescription newCurveDescription;
		newCurveDescription.addParameterDescriptions(allParameterDescriptions);

		auto quantityDescriptionCurve(std::make_unique<QuantityDescriptionCurve>());
		quantityDescriptionCurve->defineQuantityAsSingle(ot::TypeNames::getDoubleTypeName(), "");

		quantityDescriptionCurve->reserveDatapointSize(eAbs.size());
		for (size_t index = 0; index < eAbs.size(); index++)
		{
			quantityDescriptionCurve->addDatapoint(ot::Variable(eAbs[index]));
		}

		QuantityDescription* quantityDescription = quantityDescriptionCurve.release();
		quantityDescription->setName("Farfields/E_Abs" + excitationString);
		newCurveDescription.setQuantityDescription(quantityDescription);
		allCurveDescriptions.push_back(std::move(newCurveDescription));
	}
}

void ResultManager::storeCurves(const std::list<std::string>& excitationStringList, const std::list<EntityFarfieldDump*>& farfieldDumpList)
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

	std::string seriesName = createNewUniqueTopologyName(ot::FolderNames::DatasetFolder, resultSeriesName);

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
		newPlot.setStaticCurveQueryOptions(plotCfg);
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

			if (!plotName.empty() && plotPainter != nullptr)
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

	createFarfieldPlots(excitationStringList, farfieldDumpList, series, resultCollectionExtender);
}

void ResultManager::createFarfieldPlots(const std::list<std::string>& excitationStringList, const std::list<EntityFarfieldDump*>& farfieldDumpList, const MetadataSeries* series, ResultCollectionExtender& resultCollectionExtender)
{
	for (auto farfieldDump : farfieldDumpList)
	{
		EntityPropertiesSelection* typeProperty = dynamic_cast<EntityPropertiesSelection*>(farfieldDump->getProperties().getProperty("Type"));
		if (typeProperty == nullptr) continue;

		bool fixedPhi = (typeProperty->getValue() == "1D (Phi constant)");
		bool fixedTheta = (typeProperty->getValue() == "1D (Theta constant)");

		std::string title = farfieldDump->getNameOnly();
		std::string plotName = resultBasePath + "/Farfield Results/" + farfieldDump->getNameOnly();

		if (fixedPhi)
		{
			EntityPropertiesDouble* cutAngleProperty = dynamic_cast<EntityPropertiesDouble*>(farfieldDump->getProperties().getProperty("Phi cut (deg.)"));

			std::string fixedParameter = "Phi";
			std::string fixedValue = std::to_string(cutAngleProperty->getValue());

			createFarfieldPlot("E_Abs", "Theta", fixedParameter, fixedValue, title, plotName, excitationStringList, series, resultCollectionExtender);
			createFarfieldPlot("E_Theta", "Theta", fixedParameter, fixedValue, title, plotName, excitationStringList, series, resultCollectionExtender);
			createFarfieldPlot("E_Phi", "Theta", fixedParameter, fixedValue, title, plotName, excitationStringList, series, resultCollectionExtender);
		}
		else if (fixedTheta)
		{
			EntityPropertiesDouble* cutAngleProperty = dynamic_cast<EntityPropertiesDouble*>(farfieldDump->getProperties().getProperty("Theta cut (deg.)"));

			std::string fixedParameter = "Theta";
			std::string fixedValue = std::to_string(cutAngleProperty->getValue());;

			createFarfieldPlot("E_Abs", "Phi", fixedParameter, fixedValue, title, plotName, excitationStringList, series, resultCollectionExtender);
			createFarfieldPlot("E_Theta", "Phi", fixedParameter, fixedValue, title, plotName, excitationStringList, series, resultCollectionExtender);
			createFarfieldPlot("E_Phi", "Phi", fixedParameter, fixedValue, title, plotName, excitationStringList, series, resultCollectionExtender);
		}
		else
		{
			createFarfieldPlot3D("E_Abs", false, plotName, farfieldDump, excitationStringList, series, resultCollectionExtender);
			createFarfieldPlot3D("E_Theta", true, plotName, farfieldDump, excitationStringList, series, resultCollectionExtender);
			createFarfieldPlot3D("E_Phi", true, plotName, farfieldDump, excitationStringList, series, resultCollectionExtender);
		}
	}
}

void ResultManager::createFarfieldPlot3D(const std::string& type, bool complex, const std::string& plotName, EntityFarfieldDump *farfieldDump, const std::list<std::string>& excitationStringList, const MetadataSeries* series, ResultCollectionExtender& resultCollectionExtender)
{
	for (auto excitationString : excitationStringList)
	{
		EntityPropertiesDouble* thetaMinProperty = dynamic_cast<EntityPropertiesDouble*>(farfieldDump->getProperties().getProperty("Theta min (deg.)"));
		EntityPropertiesDouble* thetaMaxProperty = dynamic_cast<EntityPropertiesDouble*>(farfieldDump->getProperties().getProperty("Theta max (deg.)"));
		EntityPropertiesInteger* thetaStepsProperty = dynamic_cast<EntityPropertiesInteger*>(farfieldDump->getProperties().getProperty("Theta steps"));

		EntityPropertiesDouble* phiMinProperty = dynamic_cast<EntityPropertiesDouble*>(farfieldDump->getProperties().getProperty("Phi min (deg.)"));
		EntityPropertiesDouble* phiMaxProperty = dynamic_cast<EntityPropertiesDouble*>(farfieldDump->getProperties().getProperty("Phi max (deg.)"));
		EntityPropertiesInteger* phiStepsProperty = dynamic_cast<EntityPropertiesInteger*>(farfieldDump->getProperties().getProperty("Phi steps"));

		EntityResultFarfield newPlot(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
		newPlot.setName(plotName + "/" + type + "/" + type + excitationString);

		newPlot.setComplex(complex);
		newPlot.setFrequencies({ "15", "18" });
		newPlot.setThetaRange(thetaMinProperty->getValue(), thetaMaxProperty->getValue(), thetaStepsProperty->getValue());
		newPlot.setPhiRange(phiMinProperty->getValue(), phiMaxProperty->getValue(), phiStepsProperty->getValue());

		newPlot.createProperties();
		newPlot.setInitiallyHidden(true);

		newPlot.storeToDataBase();
		_modelComponent->addNewTopologyEntity(newPlot.getEntityID(), newPlot.getEntityStorageVersion(), false);
	}
}


void ResultManager::createFarfieldPlot(const std::string &type, const std::string &xAxis, const std::string &fixedParameter, const std::string &fixedValue, const std::string &title, const std::string &plotName, const std::list<std::string>& excitationStringList, const MetadataSeries* series, ResultCollectionExtender& resultCollectionExtender)
{
	EntityResult1DPlot newPlot(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
	newPlot.setName(plotName + "/" + type);
	newPlot.createProperties();

	ot::Plot1DCfg plotCfg;
	plotCfg.setTitle(title);
	plotCfg.setXAxisParameter(xAxis);
	newPlot.setStaticCurveQueryOptions(plotCfg);
	newPlot.storeToDataBase();
	_modelComponent->addNewTopologyEntity(newPlot.getEntityID(), newPlot.getEntityStorageVersion(), false);

	ot::PainterRainbowIterator rainbowIterator;

	for (auto excitationString : excitationStringList)
	{
		std::string curveName = plotName + "/" + type + "/" + type + excitationString;
		const MetadataQuantity* quantity = resultCollectionExtender.findMetadataQuantity("Farfields/" + type + excitationString);

		createFarfieldCurve(curveName, fixedParameter, fixedValue, rainbowIterator, series, quantity, resultCollectionExtender);
	}
}

void ResultManager::createFarfieldCurve(const std::string &curveName, const std::string& fixedParameter, const std::string& fixedValue, ot::PainterRainbowIterator &rainbowIterator, const MetadataSeries* series, const MetadataQuantity* quantity, ResultCollectionExtender& resultCollectionExtender)
{
	ot::Plot1DCurveCfg curveConfig;

	auto painter = rainbowIterator.getNextPainter();
	curveConfig.setLinePenPainter(painter.release());
	curveConfig.setLinePenWidth(2.0);

	ot::ValueComparisonDescription parameterFilter;
	parameterFilter.setName(fixedParameter);
	parameterFilter.setComparator("=");
	parameterFilter.setValue(fixedValue);
	ot::TupleInstance instance = ot::TupleDescriptionSingle::createInstance("degrees", "double");
	parameterFilter.setTupleInstance(instance);
	std::list<ot::ValueComparisonDescription> parameterFilters = { parameterFilter };

	CurveFactory::addToConfig(*series, *quantity, curveConfig, Application::instance(), &resultCollectionExtender, &parameterFilters);

	EntityResult1DCurve newCurve(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
	newCurve.setName(curveName);
	newCurve.createProperties();
	newCurve.setStaticCurveQueryOptions(curveConfig);
	newCurve.storeToDataBase();

	_modelComponent->addNewTopologyEntity(newCurve.getEntityID(), newCurve.getEntityStorageVersion(), false);
}

