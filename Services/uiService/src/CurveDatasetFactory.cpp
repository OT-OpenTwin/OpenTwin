// @otlicense
// File: CurveDatasetFactory.cpp
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

// OpenTwin header
#include "Datapoints.h"
#include "CurveColourSetter.h"
#include "CurveDatasetFactory.h"
#include "OTDataStorage/AdvancedQueryBuilder.h"
#include "AdditionalDependencies.h"
#include "ContainerFlexibleOwnership.h"
#include "OTCore/String.h"
#include "OTCore/TypeNames.h"
#include "OTCore/EntityName.h"
#include "OTCore/RuntimeTests.h"

#include "OTGui/Painter/PainterRainbowIterator.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTFrontendConnectorAPI/WindowAPI.h"
#include "OTCore/Variable/JSONToVariableConverter.h"
#include "OTDataStorage/DataLakeHelper.h"
#include "OTCore/Variable/JSONToVariableConverter.h"
#include <unordered_map>

// BSONCXX header
#include "bsoncxx/json.hpp"
#include "OTCore/MetadataHandle/MetadataQuantity.h"
#include "OTCore/MetadataHandle/MetadataSeries.h"

std::list<ot::PlotDataset*> CurveDatasetFactory::createCurves(ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _config)
{
	m_curveIDDescriptions.clear();
	const auto& queryInformation = _config.getQueryInformation();

	mongocxx::options::find options;
	std::string log;
	ot::JsonDocument entireResult = DataLakeHelper::executeQuery(_config.getDataAccessConfig(), options, log);
	std::string temp = ot::json::toJson(entireResult);

	if (!ot::json::exists(entireResult, DataLakeHelper::getDataFieldName()))
	{
		ot::WindowAPI::appendOutputMessage("Curve " + _config.getTitle() + " cannot be visualised since the query returned no data.\n");
		return {};
	}

	ot::ConstJsonArray allMongoDocuments = ot::json::getArray(entireResult, DataLakeHelper::getDataFieldName());
	uint32_t numberOfDocuments = allMongoDocuments.Size();
	if (numberOfDocuments == 0)
	{
		ot::WindowAPI::appendOutputMessage("Curve " + _config.getTitle() + " cannot be visualised since the query returned no data.\n");
		return {};
	}
	
	auto allCurvesByDependencies = createCurves(_plotCfg, _config, allMongoDocuments);
	auto datasetsByCurveTitle = createNamedCurveFamilies(allCurvesByDependencies, _config);
	auto dataSets =	createPlotDatasets(datasetsByCurveTitle,_config);
	return dataSets;
}

std::string CurveDatasetFactory::createUnitLabel(const std::string& _unit) {
	std::string result = ot::String::removePrefixSuffix(_unit, " \t\n\r");
	if (!result.empty())
	{
		result.insert(result.begin(), '(');
		result.push_back(')');
	}
	return result;
}

std::unordered_map<DependencyList, std::list<Datapoints>> CurveDatasetFactory::createCurves(ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _curveCfg, ot::ConstJsonArray& _allMongoDBDocuments)
{
	const uint32_t numberOfDocuments = _allMongoDBDocuments.Size();
	const int numberOfQuantities = 1;


	std::unordered_map<DependencyList, std::list<Datapoints>> familyOfCurves;
	std::map<std::string, std::list<std::string>> parameterValuesByParameterName;

	CurveColourSetter colourSetting(_curveCfg, numberOfQuantities);

	const std::string displayParameterLabel = _plotCfg.getXAxisParameter();

	const ot::DataLakeAccessCfg& dataLakeAccessCfg = _curveCfg.getDataAccessConfig();
	ot::JSONToVariableConverter converter;

	std::optional<ot::DataPointDecoder> decoderParameter = dataLakeAccessCfg.getFieldDecoderParameterByLabel(displayParameterLabel);
	
	std::map<std::string, ot::DataPointDecoder*> additionalParameterDecoders = dataLakeAccessCfg.getAllFieldDecoderParameterByLabel();
	additionalParameterDecoders.erase(displayParameterLabel);

	if (!decoderParameter.has_value())
	{
		throw std::exception("Plot axis selection did not work.");
	}

	ot::JSONToVariableConverter jsonToVariableConverter;
	for (uint32_t i = 0; i < numberOfDocuments; i++)
	{
		auto singleMongoDocument = ot::json::getObject(_allMongoDBDocuments, i);

		const std::string temp = ot::json::toJson(singleMongoDocument);
		// Document contains the quantity which the plot shall show
		if (ot::json::exists(singleMongoDocument, displayParameterLabel))
		{
			DependencyList dependencies;
			const std::string quantityName =	singleMongoDocument[MetadataQuantity::getFieldName().c_str()].GetString();
			std::optional<ot::DataPointDecoder> decoder = 	dataLakeAccessCfg.getFieldDecoderQuantityByLabel(quantityName);
			if(decoder.has_value())
			{
				AdditionalDependency additionalParameterInfo;
				additionalParameterInfo.m_label = MetadataQuantity::getFieldName();
				additionalParameterInfo.m_value = decoder.value().getLabel();
				additionalParameterInfo.m_unit = decoder.value().getTupleInstance().getTupleUnits().front();

				dependencies.push_back(additionalParameterInfo);
			}
			else
			{
				assert(false);
			}

			//Get series label and use it to form a unique curve name depending on the values of the document
			//Get all other dependencies and build a unique curve name depending on their values.
			
			const auto& allSeriesDecoder = dataLakeAccessCfg.getAllFieldDecoderSeriesByLabel();
			auto series = singleMongoDocument.FindMember(MetadataSeries::getFieldName().c_str());
			auto seriesDecoder = allSeriesDecoder.find(series->value.GetString());
			AdditionalDependency additionalParameterInfo;
			additionalParameterInfo.m_label = MetadataSeries::getFieldName();
			additionalParameterInfo.m_value = seriesDecoder->second->getLabel();
			//unit should already be normalised during the data point extraction

			//additionalParameterInfo.m_unit = "";
			dependencies.push_back(additionalParameterInfo);
			

			for (const auto& additionalParameter : additionalParameterDecoders)
			{
				if (ot::json::exists(singleMongoDocument, additionalParameter.first))
				{
					auto& additionalParameterEntry = singleMongoDocument[additionalParameter.first.c_str()];
					const ot::TupleInstance& parameterTuple = additionalParameter.second->getTupleInstance();

					const std::string value = ot::json::toJson(additionalParameterEntry);
					parameterValuesByParameterName[additionalParameter.first].push_back(value);

					AdditionalDependency additionalParameterInfo;
					additionalParameterInfo.m_label = additionalParameter.first;
					additionalParameterInfo.m_value = value;
					//unit should already be normalised during the data point extraction
					//additionalParameterInfo.m_unit = parameterTuple.getTupleUnits().front(); 
					dependencies.push_back(additionalParameterInfo);
				}				
			}

			//With a unique name, depending on the values of the additional parameter we now look if there is already data stored for this curve

			auto key = makeSortedKey(dependencies);
			auto curve = familyOfCurves.find(key);
			if (curve == familyOfCurves.end())
			{
				Datapoints dataPoints;
				dataPoints.reserve(numberOfDocuments - i);
				familyOfCurves[key].push_back(std::move(dataPoints));
				curve = familyOfCurves.find(key);
			}

			std::list<Datapoints>& dataPoints = curve->second;
			if (dataPoints.size() == 0)
			{
				dataPoints.push_back(Datapoints());
			}

			//Get quantity value
			const ot::JsonValue& entryQuantityValue =	singleMongoDocument["Values"];
			ot::Variable quantityValue =	jsonToVariableConverter(entryQuantityValue);
			
			// Get parameter value
			const ot::JsonValue& entryParameterValue = singleMongoDocument[displayParameterLabel.c_str()];
			double parameterValue = jsonToDouble(entryParameterValue);
		
			dataPoints.front().m_yData.push_back(quantityValue);
			dataPoints.front().m_xData.push_back(parameterValue);
		}
	}

	for (auto& curve : familyOfCurves)
	{

		std::list<Datapoints>& dataPointLists = curve.second;
		for (Datapoints& dataPoints : dataPointLists)
		{
			dataPoints.shrinkToFit();
		}
	}

	return familyOfCurves;
}

std::map<std::string, std::list<Datapoints>>  CurveDatasetFactory::createNamedCurveFamilies(std::unordered_map<DependencyList, std::list<Datapoints>>& _datasetsByDependencies, ot::Plot1DCurveCfg& _curveCfg)
{
	// Always size > 1 since at least the series is there
	// Cases for additional dependencies (parameter except of x-axis param and series):
	// 1) Only 1 additional Parameter -> Family of Curves (FoC): name of each curve shows the value of the additional parameter 
	//  1b) The additional parameter is constant -> Single curve: name of curve shows the value of the additional parameter
	// 2) More then 2 parameter -> FoC: name of each curve is abstracted to make it easier to read. The parameter values of each curve are communicated
	//  2b) All parameter are constant -> Single curve: name of curve is abstracted to make it easier to read. The parameter values of each curve are communicated
	//  2c) All but one parameter are constant -> FoC: name of each curve shows the value of the not-constant parameter. The other parameter values of each curve are communicated
	
	size_t numberOfDependencies = _datasetsByDependencies.size();
	const std::string simpleNameBase = _curveCfg.getTitle();
	std::map<std::string, std::list<Datapoints>> namedCurveFamilies;
	std::list<Datapoints>& dataPoints = _datasetsByDependencies.begin()->second;
	std::map< std::string, DependencyList> dependencyListByCurveName;
	if (numberOfDependencies == 1)
	{
		// Here all dependencies are the same for all datapoints. 
		const DependencyList& dependencies = _datasetsByDependencies.begin()->first;
		if (dependencies.size() == 2)
		{
			// Here we have the series and the quantity as dependencies. No other additional dependencies.
			//If there is only one dependency, we create the curve name such that it includes the value of the dependency.
			for(auto dependency : dependencies)
			{
				if(dependency.m_label == MetadataQuantity::getFieldName())
				{
					_curveCfg.setTitle(dependency.m_value);
				}
			}
		}
		else if (numberOfDependencies == 3)
		{

		}
		else
		{
			_curveCfg.setTitle(simpleNameBase);
		}
		namedCurveFamilies.insert({ _curveCfg.getTitle(), std::move(dataPoints)});
		dependencyListByCurveName.insert({ _curveCfg.getTitle(),dependencies });
	}
	else
	{
		int counter(1);
		double temp = std::log10(numberOfDependencies);
		uint32_t numberOfDigits = static_cast<uint32_t>(std::ceil(temp));
		for (auto entry : _datasetsByDependencies)
		{
			std::list<Datapoints>& dataPoints = entry.second;
			std::string curveNumber = ot::String::fillPrefix(std::to_string(counter), numberOfDigits, '0');
			const std::string simpleName = simpleNameBase + " (curve " + curveNumber + ")";
			counter++;

			namedCurveFamilies.insert({ simpleName,std::move(dataPoints)});
			dependencyListByCurveName.insert({ simpleName,entry.first });
		}
	}

	//Potentially for the additional info label
	std::string message =
		_curveCfg.getTitle() + ":\n";
	for (auto entry : dependencyListByCurveName)
	{
		const std::string curveName = entry.first;	
		const DependencyList& dependencies = entry.second;
		message += "Curve: " + curveName + "\n";
		for (const auto& dependency : dependencies)
		{
			message += "	" + dependency.m_label + " = " + dependency.m_value + " " + dependency.m_unit + "\n";
		}
	}
	m_curveIDDescriptions.push_back(message);
	return namedCurveFamilies;
}

std::list<ot::PlotDataset*> CurveDatasetFactory::createPlotDatasets(std::map<std::string, std::list<Datapoints>>& _curvesByCurveTitle, ot::Plot1DCurveCfg& _curveCfg)
{
	std::list<ot::PlotDataset*> dataSets;

	// We may need to iterate the rainbow painter
	CurveColourSetter curveColourSetter(_curveCfg);
	for (auto& singleCurve : _curvesByCurveTitle)
	{
		const std::string curveTitle = singleCurve.first;
		ot::Plot1DCurveCfg newCurveCfg = _curveCfg;
		newCurveCfg.setTitle(curveTitle);
		auto curveData = singleCurve.second.begin();
				
		ot::PlotDatasetData datasetData;
		if (curveData->m_yData.front().isComplex())
		{
			std::vector<std::complex<double>> complexData = toComplexVector(curveData->m_yData);
			datasetData = ot::PlotDatasetData(std::move(curveData->m_xData), std::move(complexData));
		}
		else
		{
			std::vector<double> doubleData = toDoubleVector(curveData->m_yData);
			datasetData = ot::PlotDatasetData(std::move(curveData->m_xData), std::move(doubleData));
		}
		
		auto dataset = new ot::PlotDataset(nullptr, newCurveCfg, std::move(datasetData));
		dataset->setCurveNameBase(curveTitle);
		//OT_LOG_T("Curve created { \"Title\": \"" + curveTitle + "\", \"EntityName\": \"" + dataset->getEntityName() + "\", \"DatasetTitle\": \"" + dataset->getConfig().getTitle() + "\" }");
		dataSets.push_back(dataset);

	}
	return dataSets;
}

double CurveDatasetFactory::jsonToDouble(const std::string& _memberName, ot::ConstJsonObject& _jsonEntry, const std::string& _dataType)
{
	double value;
	if (_dataType == ot::TypeNames::getFloatTypeName()) {
		float serialisedVal = ot::json::getFloat(_jsonEntry, _memberName);
		value =	static_cast<double>(serialisedVal);
	}
	else if (_dataType== ot::TypeNames::getDoubleTypeName()) {
		value = ot::json::getDouble(_jsonEntry,_memberName);
	}
	else if (_dataType == ot::TypeNames::getInt32TypeName()) {
		int32_t serialisedVal = ot::json::getInt(_jsonEntry, _memberName);
		value = static_cast<double>(serialisedVal);
	}
	else if (_dataType == ot::TypeNames::getInt64TypeName()) {
		int64_t serialisedVal = ot::json::getInt64(_jsonEntry, _memberName);
		value = static_cast<double>(serialisedVal);
	}
	else
	{
		throw std::invalid_argument("Curve data has not supported type: " + _dataType);
	}

	return value;
}

double CurveDatasetFactory::jsonToDouble(const rapidjson::Value& _jsonEntry, const std::string& _dataType)
{
	double value;
	if (_dataType == ot::TypeNames::getFloatTypeName()) {
		float serialisedVal = _jsonEntry.GetFloat();
		value = static_cast<double>(serialisedVal);
	}
	else if (_dataType == ot::TypeNames::getDoubleTypeName()) {
		value = _jsonEntry.GetDouble();
	}
	else if (_dataType == ot::TypeNames::getInt32TypeName()) {
		int32_t serialisedVal = _jsonEntry.GetInt();
		value = static_cast<double>(serialisedVal);
	}
	else if (_dataType == ot::TypeNames::getInt64TypeName()) {
		int64_t serialisedVal = _jsonEntry.GetInt64();
		value = static_cast<double>(serialisedVal);
	}
	else
	{
		throw std::invalid_argument("Curve data has not supported type: " + _dataType);
	}

	return value;
}

double CurveDatasetFactory::jsonToDouble(const rapidjson::Value& _jesonEntry)
{
	if (_jesonEntry.IsFloat()) {
		float serialisedVal = _jesonEntry.GetFloat();
		return static_cast<double>(serialisedVal);
	}
	else if (_jesonEntry.IsDouble()) {
		return _jesonEntry.GetDouble();
	}
	else if (_jesonEntry.IsInt()) {
		int32_t serialisedVal = _jesonEntry.GetInt();
		return static_cast<double>(serialisedVal);
	}
	else if (_jesonEntry.IsInt64()) {
		int64_t serialisedVal = _jesonEntry.GetInt64();
		return static_cast<double>(serialisedVal);
	}
	else
	{
		throw std::invalid_argument("Curve data has not supported type");
	}
}

ot::PlotDatasetData CurveDatasetFactory::createCurveData(const std::vector<double>& _xData, const std::vector<ot::Variable>& _yData, bool _yDataIsComplex, const ot::Plot1DCfg& _plotCfg) {
	ot::PlotDatasetData result;

	if (_yDataIsComplex) {
		result = ot::PlotDatasetData(_xData, toComplexVector(_yData), _plotCfg.getXAxisQuantity(), _plotCfg.getYAxisQuantity());
	}
	else {
		result = ot::PlotDatasetData(_xData, toDoubleVector(_yData));
	}

	return result;
}

std::vector<double> CurveDatasetFactory::toDoubleVector(const std::vector<ot::Variable>& _values) {
	std::vector<double> result;

	for (const auto& value : _values)
	{
		if (value.isInt32()) {
			result.push_back(static_cast<double>(value.getInt32()));
		}
		else if (value.isInt64()) {
			result.push_back(static_cast<double>(value.getInt64()));
		}
		else if (value.isFloat()) {
			result.push_back(static_cast<double>(value.getFloat()));
		}
		else if (value.isDouble()) {
			result.push_back(value.getDouble());
		}
		else if (value.isComplex()) {
			result.push_back(value.getComplex().real());
		}
		else {
			OTAssert(0, "Expected numerical value");
			result.push_back(0.);
		}
	}

	return result;
}

std::vector<std::complex<double>> CurveDatasetFactory::toComplexVector(const std::vector<ot::Variable>& _values) {
	std::vector<std::complex<double>> result;

	for (const auto& value : _values)
	{
		OTAssert(value.isComplex(), "Expected complex value");
		result.push_back(value.getComplex());
	}

	return result;
}
