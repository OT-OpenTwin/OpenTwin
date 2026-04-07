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
#include "AppBase.h"
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
	auto matchingDependenciesInfo = findMatchingDependencies(allCurvesByDependencies, _config);
	auto datasetsByCurveTitle = createNamedCurves(std::move(allCurvesByDependencies), _config, std::move(matchingDependenciesInfo));
	auto dataSets =	createPlotDatasets(_plotCfg, std::move(datasetsByCurveTitle), _config);
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

CurveDatasetFactory::DependencyDataMap CurveDatasetFactory::createCurves(const ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _curveCfg, ot::ConstJsonArray& _allMongoDBDocuments)
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

			// Get series label and use it to form a unique curve name depending on the values of the document
			// Get all other dependencies and build a unique curve name depending on their values.
			
			const auto& allSeriesDecoder = dataLakeAccessCfg.getAllFieldDecoderSeriesByLabel();
			auto series = singleMongoDocument.FindMember(MetadataSeries::getFieldName().c_str());
			auto seriesDecoder = allSeriesDecoder.find(series->value.GetString());
			AdditionalDependency additionalParameterInfo;
			additionalParameterInfo.m_label = MetadataSeries::getFieldName();
			additionalParameterInfo.m_value = seriesDecoder->second->getLabel();
			//unit should already be normalised during the data point extraction

			// additionalParameterInfo.m_unit = "";
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

			// With a unique name, depending on the values of the additional parameter we now look if there is already data stored for this curve

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

			// Get quantity value
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

CurveDatasetFactory::CurveByTitleMap CurveDatasetFactory::createNamedCurves(DependencyDataMap&& _datasetsByDependencies, ot::Plot1DCurveCfg& _curveCfg, MatchingDependencies&& _matchingDependenciesInfo)
{
	if (_datasetsByDependencies.empty())
	{
		return CurveByTitleMap();
	}

	size_t numberOfDependencies = _datasetsByDependencies.size();

	if (numberOfDependencies == 1)
	{
		// Only one curve -> simple naming
		return createNamedCurvesSimpleNames(std::move(_datasetsByDependencies), _curveCfg);
	}

	MatchingDependencies matchingDependencies;
	MatchingDependencies nonMatchingDependencies;
	for (MatchingDependency& dep : _matchingDependenciesInfo)
	{
		if (dep.isMatching)
		{
			matchingDependencies.push_back(std::move(dep));
		}
		else
		{
			nonMatchingDependencies.push_back(std::move(dep));
		}
	}

	if (nonMatchingDependencies.empty())
	{
		// All dependencies are matching -> simple naming
		return createNamedCurvesSimpleNames(std::move(_datasetsByDependencies), _curveCfg);
	}
	else if (nonMatchingDependencies.size() == 1)
	{
		// Only one dependency is not matching -> use it for naming

		return createNamedCurvesByDependency(std::move(_datasetsByDependencies), _curveCfg, nonMatchingDependencies.front().dependency.m_label);
	}
	else
	{
		MatchingDependencies nonMatchingParameterDependencies;
		bool hasSeriesDiff = false;
		bool hasQuantityDiff = false;

		for (const auto& dep : nonMatchingDependencies)
		{
			if (dep.type == DependencyType::Parameter)
			{
				nonMatchingParameterDependencies.push_back(dep);
			}
			else if (dep.type == DependencyType::Quantity)
			{
				hasQuantityDiff = true;
			}
			else if (dep.type == DependencyType::Series)
			{
				hasSeriesDiff = true;
			}
		}

		std::string paramKey;
		if (hasSeriesDiff) {
			paramKey = MetadataSeries::getFieldName();
		}
		else if (hasQuantityDiff)
		{
			paramKey = MetadataQuantity::getFieldName();
		}

		if (nonMatchingParameterDependencies.size() == 1)
		{
			paramKey = nonMatchingParameterDependencies.front().dependency.m_label;
		}

		if (paramKey.empty())
		{
			return createNamedCurvesSimpleNames(std::move(_datasetsByDependencies), _curveCfg);
		}
		else
		{
			return createNamedCurvesByDependency(std::move(_datasetsByDependencies), _curveCfg, paramKey);
		}
	}

	/*

	// Potentially for the additional info label
	std::string message = _curveCfg.getTitle() + ":\n";

	for (const auto& entry : dependencyListByCurveName)
	{
		const std::string& curveName = entry.first;	
		const DependencyList& dependencies = entry.second;
		message += "Curve: " + curveName + "\n";
		for (const auto& dependency : dependencies)
		{
			message += "	" + dependency.m_label + " = " + dependency.m_value + " " + dependency.m_unit + "\n";
		}
	}

	m_curveIDDescriptions.push_back(message);

	*/
}

CurveDatasetFactory::CurveByTitleMap CurveDatasetFactory::createNamedCurvesSimpleNames(DependencyDataMap&& _datasetsByDependencies, ot::Plot1DCurveCfg& _curveCfg)
{
	CurveByTitleMap result;

	if (!_datasetsByDependencies.empty())
	{
		size_t numberOfCurves = 0;
		for (const auto& entry : _datasetsByDependencies)
		{
			numberOfCurves += entry.second.size();
		}

		if (numberOfCurves == 1)
		{
			// Only one curve -> use the title as curve name

			for (auto& entry : _datasetsByDependencies)
			{
				std::list<Datapoints>& dataPointsList = entry.second;
				for (Datapoints& dataPoints : dataPointsList)
				{
					std::string title = _curveCfg.getTitle();
					std::pair<Datapoints, DependencyList> dataDependencyPair = { std::move(dataPoints), entry.first };
					result.insert({ std::move(title), std::move(dataDependencyPair)});
					break;
				}
			}
		}
		else
		{
			int counter = 1;
			const double temp = std::log10(numberOfCurves);
			const uint32_t numberOfDigits = static_cast<uint32_t>(std::ceil(temp));

			for (auto& entry : _datasetsByDependencies)
			{
				std::list<Datapoints>& dataPointsList = entry.second;
				for (Datapoints& dataPoints : dataPointsList)
				{
					std::string curveNumber = ot::String::fillPrefix(std::to_string(counter), numberOfDigits, '0');
					const std::string title = _curveCfg.getTitle() + " (curve " + curveNumber + ")";
					counter++;

					std::pair<Datapoints, DependencyList> dataDependencyPair = { std::move(dataPoints), entry.first };
					result.insert({ title, std::move(dataDependencyPair) });
				}
			}
		}
	}

	return result;
}

CurveDatasetFactory::CurveByTitleMap CurveDatasetFactory::createNamedCurvesByDependency(DependencyDataMap&& _datasetsByDependencies, ot::Plot1DCurveCfg& _curveCfg, const std::string& _dependencyLabel)
{
	std::map<std::string, std::pair<std::list<Datapoints>, DependencyList>> tmpResult;
	
	for (auto& entry : _datasetsByDependencies)
	{
		AdditionalDependency dependency = findDependency(entry.first, _dependencyLabel);

		std::string title = _curveCfg.getTitle();

		if (dependency.m_label == MetadataQuantity::getFieldName() || dependency.m_label == MetadataSeries::getFieldName())
		{
			title = _curveCfg.getTitle() + " (" + dependency.m_value + (dependency.m_unit.empty() ? "" : " (" + dependency.m_unit + ")") + ")";
		}
		else
		{
			title = _curveCfg.getTitle() + " (" + dependency.m_label + " = " + dependency.m_value + (dependency.m_unit.empty() ? "" : " (" + dependency.m_unit + ")") + ")";
		}

		std::list<Datapoints>& dataPoints = entry.second;

		auto it = tmpResult.find(title);
		if (it != tmpResult.end())
		{
			it->second.first.splice(it->second.first.end(), std::move(dataPoints));
		}
		else
		{
			std::pair<std::list<Datapoints>, DependencyList> dataDependencyPair = { std::move(dataPoints), entry.first };
			tmpResult.insert({ title, std::move(dataDependencyPair) });
		}
	}

	CurveByTitleMap result;

	// Make names unique
	for (auto& entry : tmpResult)
	{
		std::list<Datapoints>& dataPointsList = entry.second.first;

		int counter = 0;

		const std::string& titleBase = entry.first;

		bool isSingleCurve = dataPointsList.size() == 1;

		for (Datapoints& dataPoints : dataPointsList)
		{
			std::string curveTitle = titleBase;

			bool isNameUnique = (isSingleCurve || (result.find(curveTitle) == result.end()));
			while (!isNameUnique)
			{
				counter++;
				curveTitle = titleBase + " (curve " + std::to_string(counter) + ")";
				isNameUnique = (result.find(curveTitle) == result.end());
			}

			std::pair<Datapoints, DependencyList> dataDependencyPair = { std::move(dataPoints), entry.second.second };
			result.insert({ std::move(curveTitle), std::move(dataDependencyPair) });
		}
	}

	return result;
}

std::list<ot::PlotDataset*> CurveDatasetFactory::createPlotDatasets(const ot::Plot1DCfg& _plotCfg, CurveByTitleMap&& _curvesByCurveTitle, ot::Plot1DCurveCfg& _curveCfg)
{
	std::list<ot::PlotDataset*> dataSets;

	// We may need to iterate the rainbow painter
	CurveColourSetter curveColourSetter(_curveCfg);
	for (auto& singleCurve : _curvesByCurveTitle)
	{
		const std::string curveTitle = singleCurve.first;
		ot::Plot1DCurveCfg newCurveCfg = _curveCfg;
		newCurveCfg.setTitle(curveTitle);

		const std::string toolTip = getCurveToolTip(newCurveCfg.getEntityName(), singleCurve.second.second);
		newCurveCfg.setToolTip(toolTip);

		AppBase::instance()->appendOutputMessage("\n" + curveTitle + "\n" + toolTip + "\n");

		Datapoints& curveData = singleCurve.second.first;
				
		ot::PlotDatasetData datasetData;
		if (curveData.m_yData.front().isComplex())
		{
			std::vector<std::complex<double>> complexData = toComplexVector(curveData.m_yData);
			datasetData = ot::PlotDatasetData(std::move(curveData.m_xData), std::move(complexData), _plotCfg.getXAxisQuantity(), _plotCfg.getYAxisQuantity());
		}
		else
		{
			std::vector<double> doubleData = toDoubleVector(curveData.m_yData);
			datasetData = ot::PlotDatasetData(std::move(curveData.m_xData), std::move(doubleData));
		}
		
		auto dataset = new ot::PlotDataset(nullptr, newCurveCfg, std::move(datasetData));
		dataset->setCurveNameBase(curveTitle);

		//OT_LOG_T("Curve created { \"Title\": \"" + curveTitle + "\", \"EntityName\": \"" + dataset->getEntityName() + "\", \"DatasetTitle\": \"" + dataset->getConfig().getTitle() + "\" }");
		dataSets.push_back(dataset);

	}
	return dataSets;
}

AdditionalDependency CurveDatasetFactory::findDependency(const DependencyList& _dependencies, const std::string& _label)
{
	for (const AdditionalDependency& dependency : _dependencies)
	{
		if (dependency.m_label == _label)
		{
			return dependency;
		}
	}

	throw ot::Exception::ObjectNotFound("Dependency with label " + _label + " not found");
}

CurveDatasetFactory::MatchingDependencies CurveDatasetFactory::findMatchingDependencies(const DependencyDataMap& _datasetsByDependencies, ot::Plot1DCurveCfg& _curveCfg)
{
	std::vector<MatchingDependency> result;

	if (!_datasetsByDependencies.empty())
	{
		// Create a vector that holds all dependencies and whether they are the same for all curves.
		const DependencyList& firstDependencyList = _datasetsByDependencies.begin()->first;

		result.reserve(firstDependencyList.size());

		size_t numberOfCurves = 0;
		for (const auto& entry : _datasetsByDependencies)
		{
			numberOfCurves += entry.second.size();
		}

		for (const AdditionalDependency& dependency : firstDependencyList)
		{
			MatchingDependency newMatchInfo(dependency);
			newMatchInfo.isMatching = (numberOfCurves > 1);
			result.push_back(std::move(newMatchInfo));
		}

		// Check for matching dependencies for all curves
		for (const auto& entry : _datasetsByDependencies)
		{
			auto resultIt = result.begin();
			while (resultIt != result.end())
			{
				bool found = false;
				for (const auto& dependency : entry.first)
				{
					// Dependency is the same
					if (resultIt->dependency.m_label == dependency.m_label)
					{
						found = true;

						// Chekc equal value
						if (resultIt->dependency.m_value != dependency.m_value)
						{
							resultIt->isMatching = false;
						}
					}

					// Dependency not found
					if (found)
					{
						++resultIt;
					}
					else
					{
						resultIt = result.erase(resultIt);
						if (resultIt == result.end())
						{
							break;
						}
					}
				}
			}
		}
	}

	return result;
}

CurveDatasetFactory::DependencyType CurveDatasetFactory::getDependencyType(const AdditionalDependency& _dependency)
{
	return getDependencyType(_dependency.m_label, _dependency.m_unit);
}

CurveDatasetFactory::DependencyType CurveDatasetFactory::getDependencyType(const std::string& _label, const std::string& _unit)
{
	if (_label == MetadataSeries::getFieldName())
	{
		return DependencyType::Series;
	}
	else if (_label == MetadataQuantity::getFieldName())
	{
		return DependencyType::Quantity;
	}
	else
	{
		return DependencyType::Parameter;
	}
}

std::string CurveDatasetFactory::getCurveToolTip(const std::string& _entityName, const DependencyList& _dependencies)
{
	std::string toolTip = _entityName;
	//std::string toolTip = ot::EntityName::getSubName(_entityName).value();

	for (const auto& dependency : _dependencies)
	{
		toolTip.append("\n    " + dependency.m_label + " = " + dependency.m_value + (dependency.m_unit.empty() ? "" : " (" + dependency.m_unit + ")"));
	}

	return toolTip;
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
