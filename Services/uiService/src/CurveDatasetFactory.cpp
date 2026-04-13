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
#include "ContainerFlexibleOwnership.h"
#include "OTCore/String.h"
#include "OTCore/TypeNames.h"
#include "OTCore/EntityName.h"
#include "OTCore/RuntimeTests.h"
#include "OTCore/ContainerHelper.h"

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

std::list<ot::PlotDataset*> CurveDatasetFactory::createCurves(ot::Plot1DCfg& _plotCfg, const ot::Plot1DCurveCfg& _curveCfg)
{
	m_curveIDDescriptions.clear();
	
	// We check the validity of the access cfg first
	const ot::DataLakeAccessCfg& accessCfg = _curveCfg.getDataAccessConfig();
	if (accessCfg.getAllFieldDecoderSeriesByLabel().size() == 0)
	{
		ot::WindowAPI::appendOutputMessage("Curve " + _curveCfg.getTitle() + " cannot be visualised since no series matches the criteria.\n");
		return {};
	}

	//@Alex here you can get the selected parameter and its units. Parameter never have more then 1 units. If available, it is the first one.
	/*const std::string xAxisParameter;
	accessCfg.getAllFieldDecoderParameter().find(xAxisParameter)->second.getTupleInstance().getTupleUnits().front();*/

	mongocxx::options::find options;
	std::string log;
	ot::JsonDocument entireResult = DataLakeHelper::executeQuery(_curveCfg.getDataAccessConfig(), options, log);
	std::string temp = ot::json::toJson(entireResult);

	if (!ot::json::exists(entireResult, DataLakeHelper::getDataFieldName()))
	{
		ot::WindowAPI::appendOutputMessage("Curve " + _curveCfg.getTitle() + " cannot be visualised since the query returned no data.\n");
		return {};
	}

	ot::ConstJsonArray allMongoDocuments = ot::json::getArray(entireResult, DataLakeHelper::getDataFieldName());
	uint32_t numberOfDocuments = allMongoDocuments.Size();
	if (numberOfDocuments == 0)
	{
		ot::WindowAPI::appendOutputMessage("Curve " + _curveCfg.getTitle() + " cannot be visualised since the query returned no data.\n");
		return {};
	}
	
	auto allCurvesByDependencies = createCurves(_plotCfg, _curveCfg, allMongoDocuments);
	auto dataSets =	createPlotDatasets(_plotCfg, std::move(allCurvesByDependencies), _curveCfg);
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

CurveDatasetFactory::DependencyInfoList CurveDatasetFactory::createCurves(const ot::Plot1DCfg& _plotCfg, const ot::Plot1DCurveCfg& _curveCfg, ot::ConstJsonArray& _allMongoDBDocuments)
{
	const uint32_t numberOfDocuments = _allMongoDBDocuments.Size();
	const int numberOfQuantities = 1;

	DependencyInfoList familyOfCurves;
	std::map<std::string, std::list<std::string>> parameterValuesByParameterName;

	CurveColourSetter colourSetting(_curveCfg, numberOfQuantities);

	const std::string displayParameterLabel = _plotCfg.getXAxisParameter();

	const ot::DataLakeAccessCfg& dataLakeAccessCfg = _curveCfg.getDataAccessConfig();
	
	/*
	std::optional<ot::DataPointDecoder> decoderParameter = dataLakeAccessCfg.getFieldDecoderParameterByLabel(displayParameterLabel);
	
	std::map<std::string, ot::DataPointDecoder*> additionalParameterDecoders = dataLakeAccessCfg.getAllFieldDecoderParameterByLabel();
	additionalParameterDecoders.erase(displayParameterLabel);

	if (!decoderParameter.has_value())
	{
		//throw std::exception("Plot axis selection did not work.");
	}*/

	ot::JSONToVariableConverter jsonToVariableConverter;
	for (uint32_t i = 0; i < numberOfDocuments; i++)
	{
		auto singleMongoDocument = ot::json::getObject(_allMongoDBDocuments, i);

		const std::string temp = ot::json::toJson(singleMongoDocument);
		// Document contains the quantity which the plot shall show
		if (ot::json::exists(singleMongoDocument, displayParameterLabel))
		{
			ot::DatasetDependencyInfos dependencies;
			const std::string quantityName = singleMongoDocument[MetadataQuantity::getFieldName().c_str()].GetString();
			std::optional<ot::DataPointDecoder> decoder = dataLakeAccessCfg.getFieldDecoderQuantityByLabel(quantityName);
			if(decoder.has_value())
			{
				ot::DatasetDependencyInfo additionalParameterInfo;
				additionalParameterInfo.setLabel(MetadataQuantity::getFieldName());
				additionalParameterInfo.setValue(decoder.value().getLabel());
				additionalParameterInfo.setUnit(decoder.value().getTupleInstance().getTupleUnits().front());

				dependencies.addDependency(additionalParameterInfo);
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

			ot::DatasetDependencyInfo additionalParameterInfo;
			additionalParameterInfo.setLabel(MetadataSeries::getFieldName());
			additionalParameterInfo.setValue(seriesDecoder->second->getLabel());
			// unit should already be normalised during the data point extraction

			dependencies.addDependency(additionalParameterInfo);
			
			//for (const auto& additionalParameter : additionalParameterDecoders)
			//{
			//	if (ot::json::exists(singleMongoDocument, additionalParameter.first))
			//	{
			//		auto& additionalParameterEntry = singleMongoDocument[additionalParameter.first.c_str()];
			//		const ot::TupleInstance& parameterTuple = additionalParameter.second->getTupleInstance();

			//		const std::string value = ot::json::toJson(additionalParameterEntry);
			//		parameterValuesByParameterName[additionalParameter.first].push_back(value);

			//		ot::DatasetDependencyInfo additionalParameterInfo;
			//		additionalParameterInfo.setLabel(additionalParameter.first);
			//		additionalParameterInfo.setValue(value);
			//		//unit should already be normalised during the data point extraction
			//		//additionalParameterInfo.m_unit = parameterTuple.getTupleUnits().front(); 
			//		dependencies.addDependency(additionalParameterInfo);
			//	}				
			//}



			Datapoints* dataPoints = nullptr;

			for (auto& curve : familyOfCurves)
			{
				if (curve.first == dependencies)
				{
					dataPoints = &curve.second;
					break;
				}
			}

			if (dataPoints == nullptr)
			{
				DependencyInfoEntry infoEntry = std::make_pair(dependencies, Datapoints());
				familyOfCurves.push_back(std::move(infoEntry));
				dataPoints = &familyOfCurves.back().second;
				dataPoints->reserve(numberOfDocuments - i);
			}
			
			OTAssertNullptr(dataPoints);

			// Get quantity value
			const ot::JsonValue& entryQuantityValue =	singleMongoDocument["Values"];
			ot::Variable quantityValue =	jsonToVariableConverter(entryQuantityValue);
			
			// Get parameter value
			const ot::JsonValue& entryParameterValue = singleMongoDocument[displayParameterLabel.c_str()];
			double parameterValue = jsonToDouble(entryParameterValue);

			dataPoints->m_yData.push_back(quantityValue);
			dataPoints->m_xData.push_back(parameterValue);
		}
	}

	for (auto& curve : familyOfCurves)
	{
		curve.second.shrinkToFit();
	}

	return familyOfCurves;
}

std::list<ot::PlotDataset*> CurveDatasetFactory::createPlotDatasets(const ot::Plot1DCfg& _plotCfg, DependencyInfoList&& _curveData, const ot::Plot1DCurveCfg& _curveCfg)
{

	std::list<ot::PlotDataset*> dataSets;

	CurveColourSetter curveColourSetter(_curveCfg);

	for (auto& curve : _curveData)
	{
		const ot::DatasetDependencyInfos& dependencies = curve.first;
		auto& dataPoints = curve.second;

		// @Alex Here you can get metadata per series. You can turn these metadata as json document into instances of additional dependencies for the naming. 
		/*auto accessCfg = _curveCfg.getDataAccessConfig();
		for (auto dep : dependencies.getDependencies())
		{
			bool isSeries;
			if (isSeries)
			{
				auto seriesName = dep.getLabel();
				const ot::JsonDocument& doc = accessCfg.getSeriesMetadata(seriesName);
			}
		}*/

		ot::PlotDatasetData datasetData;
		if (dataPoints.m_yData.front().isComplex())
		{
			std::vector<std::complex<double>> complexData = toComplexVector(dataPoints.m_yData);
			datasetData = ot::PlotDatasetData(std::move(dataPoints.m_xData), std::move(complexData), _plotCfg.getXAxisQuantityComponent(), _plotCfg.getYAxisQuantityComponent());
		}
		else
		{
			std::vector<double> doubleData = toDoubleVector(dataPoints.m_yData);
			datasetData = ot::PlotDatasetData(std::move(dataPoints.m_xData), std::move(doubleData));
		}

		ot::PlotDataset* dataset = new ot::PlotDataset(nullptr, _curveCfg, std::move(datasetData));
		dataset->setDependencyInfos(dependencies);
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
		result = ot::PlotDatasetData(_xData, toComplexVector(_yData), _plotCfg.getXAxisQuantityComponent(), _plotCfg.getYAxisQuantityComponent());
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
