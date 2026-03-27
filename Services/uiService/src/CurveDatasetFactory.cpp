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
#include "ShortParameterDescription.h"
#include "ContainerFlexibleOwnership.h"
#include "OTCore/String.h"
#include "OTCore/TypeNames.h"
#include "OTCore/EntityName.h"
#include "OTCore/RuntimeTests.h"
#include "OTCore/Variable/ExplicitStringValueConverter.h"
#include "OTGui/Painter/PainterRainbowIterator.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTFrontendConnectorAPI/WindowAPI.h"
#include "OTCore/Variable/JSONToVariableConverter.h"
#include "OTDataStorage/DataLakeHelper.h"

// BSONCXX header
#include "bsoncxx/json.hpp"

std::list<ot::PlotDataset*> CurveDatasetFactory::createCurves(ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _config, const std::string& _xAxisParameter, const std::list<ot::ValueComparisonDescription>& _valueComparisons)
{
	m_curveIDDescriptions.clear();
	const auto& queryInformation = _config.getQueryInformation();

	mongocxx::options::find options;
	 
	ot::JsonDocument entireResult = DataLakeHelper::executeQuery(_config.getDataAccessConfig(), options);
	if (!ot::json::exists(entireResult, DataLakeHelper::getDataFieldName()))
	{
		ot::WindowAPI::appendOutputMessage("Curve " + _config.getTitle() + " cannot be visualised since the query returned no data.\n");
		return {};
	}
	ot::ConstJsonArray allMongoDocuments = ot::json::getArray(entireResult, DataLakeHelper::getDataFieldName());
	if (allMongoDocuments.Size() == 0)
	{
		ot::WindowAPI::appendOutputMessage("Curve " + _config.getTitle() + " cannot be visualised since the query returned no data.\n");
		return {};
	}

	CurveType curveType = determineCurveType(queryInformation);
	
	std::list<ot::PlotDataset*> dataSets;
	if (curveType == CurveType::m_single)
	{
		dataSets = createSingleCurve(_plotCfg,_config, allMongoDocuments);
	}
	else
	{
		assert(curveType == CurveType::m_familyCurve);
		dataSets = createCurveFamily(_plotCfg, _config, _xAxisParameter, allMongoDocuments);
	}
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

ot::JsonDocument CurveDatasetFactory::queryCurveData(const ot::QueryInformation& _queryInformation, const std::list<ot::ValueComparisonDescription>& _valueComparisons)
{
	//First we find the valid value comparisons give them additional information from the query information
	std::list<ot::ValueComparisonDescription> validQueries = extractValidValueDescriptions(_queryInformation, _valueComparisons);

	//Now we build the queries from the set properties
	AdvancedQueryBuilder queryBuilder;
	std::list<BsonViewOrValue> additionalComparisons;
	for (ot::ValueComparisonDescription& valueComparison : validQueries)
	{
		const std::string type = valueComparison.getTupleInstance().getTupleElementDataTypes().front();
		const std::string value = valueComparison.getValue();
		BsonViewOrValue comparison = queryBuilder.createComparison(valueComparison);
		additionalComparisons.push_back(std::move(comparison));
	}

	//Now we assemble the final query
	bsoncxx::document::value docProjection = bsoncxx::from_json(_queryInformation.getProjection());
	bsoncxx::document::view_or_value projection(docProjection);

	bsoncxx::document::value docQuery = bsoncxx::from_json(_queryInformation.getQuery());
	bsoncxx::document::view_or_value query (docQuery);
	if (additionalComparisons.size() > 0)
	{
		additionalComparisons.push_back(query);
		query = queryBuilder.connectWithAND(std::move(additionalComparisons));
	}



	const std::string queryTmp = bsoncxx::to_json(query);
	const std::string projectionTmp = bsoncxx::to_json(projection);
	DataStorageAPI::DataStorageResponse dbResponse = m_dataAccess.searchInDataLakePartition(query, projection, 0);

	if (dbResponse.getSuccess()) 
	{
		const std::string queryResponse = dbResponse.getResult();
		ot::JsonDocument doc;
		doc.fromJson(queryResponse);
		return doc;
	}
	else
	{
		throw std::exception(("Failed to query data with DB message: " + dbResponse.getMessage()).c_str());
	}
}

const std::list<ot::ValueComparisonDescription> CurveDatasetFactory::extractValidValueDescriptions(const ot::QueryInformation& _queryInformation, const std::list<ot::ValueComparisonDescription>& _valueComparisons)
{
	std::list<ot::ValueComparisonDescription> validValueDescriptions;
	for (const ot::ValueComparisonDescription& queryDefinition : _valueComparisons)
	{
		if (_queryInformation.getQuantityDescription().getLabel() == queryDefinition.getName())
		{
			const auto& quantityDescription = _queryInformation.getQuantityDescription();
			auto validValueComparison = createValidValueComparison(quantityDescription, queryDefinition);
			if (validValueComparison.has_value())
			{
				validValueDescriptions.push_back(validValueComparison.value());
			}
		}
		else
		{
			for (const auto& parameterDescription : _queryInformation.getParameterDescriptions())
			{
				if (parameterDescription.getLabel() == queryDefinition.getName())
				{
					auto validValueComparison = createValidValueComparison(parameterDescription, queryDefinition);
					if (validValueComparison.has_value())
					{
						validValueDescriptions.push_back(validValueComparison.value());
					}
					break;
				}
			}
		}
	}
	return validValueDescriptions;
}



CurveDatasetFactory::CurveType CurveDatasetFactory::determineCurveType(const ot::QueryInformation& _queryInformation)
{
	size_t numberOfParameter = _queryInformation.getParameterDescriptions().size();
	if (numberOfParameter == 0)
	{
		throw std::exception("Trying to visualise a curve with no parameter");
	}
	else if (numberOfParameter == 1)
	{
		return CurveType::m_single;
	}
	else 
	{
		assert(numberOfParameter > 1);
		return CurveType::m_familyCurve;
	}
}

std::list <ot::PlotDataset*> CurveDatasetFactory::createSingleCurve(ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _curveCfg, ot::ConstJsonArray& _allMongoDBDocuments)
{
	const uint32_t numberOfDocuments = _allMongoDBDocuments.Size();

	const ot::QueryInformation& queryInformation =_curveCfg.getQueryInformation();
	const ot::DataPointDecoder& quantityInformation = queryInformation.getQuantityDescription();
	assert(queryInformation.getParameterDescriptions().size() == 1); // For a single curve there should be only one parameter

	std::vector<uint32_t> quantityDimensions = quantityInformation.getDimension();
	uint32_t numberOfQuantityEntries(1);
	for (uint32_t quantityDimension : quantityDimensions)
	{
		numberOfQuantityEntries *= quantityDimension;
	}

	// We may need to iterate the rainbow painter
	CurveColourSetter colourSetting(_curveCfg, numberOfQuantityEntries);
	
	// Initialize data vectors
	std::vector<double> dataX;
	dataX.reserve(numberOfDocuments);

	std::vector<std::pair<std::vector<ot::Variable>, bool>> dataY; // Holds y values for each curve, bool indicates if the curve is complex only
	dataY.reserve(numberOfQuantityEntries);
	
	for (int i = 0; i < numberOfQuantityEntries; i++)
	{
		dataY.emplace_back(std::vector<ot::Variable>(), true);
		dataY.back().first.reserve(numberOfDocuments);
	}

	auto entryDescription = queryInformation.getParameterDescriptions().begin();

	const ot::TupleInstance& tupleInstance = quantityInformation.getTupleInstance();

	for (uint32_t i = 0; i < numberOfDocuments; i++) {
		auto singleMongoDocument = ot::json::getObject(_allMongoDBDocuments, i);		

		// Get quantity value
		if (numberOfQuantityEntries == 1) {
			ot::JSONToVariableConverter converter; 
			auto valueJson  = singleMongoDocument.FindMember(quantityInformation.getFieldName().c_str());
			const std::string temp = ot::json::toJson(valueJson->value);
			ot::Variable value = converter(valueJson->value);
			if (!value.isComplex()) {
				dataY.front().second = false;
				dataY.front().first.push_back(std::move(value));
			}
			else {
				// recalculate (we currently get mag/phase)
				// todo ^---
				dataY.front().first.push_back(std::move(value));
			}
			
		}
		else {
			ot::ConstJsonArray matrix = ot::json::getArray(singleMongoDocument, quantityInformation.getFieldName());
			assert(matrix.Size() == numberOfQuantityEntries);
			for (uint32_t j = 0; j < numberOfQuantityEntries; j++)
			{
				const rapidjson::Value& matrixEntry = matrix[j];
				ot::Variable quantityValue = jsonToDouble(matrixEntry, tupleInstance.getTupleElementDataTypes().front());
				dataY[j].first.push_back(std::move(quantityValue));
				dataY[j].second = false;
			}
		}

		// Get parameter value
		double parameterValue = jsonToDouble(entryDescription->getFieldName(), singleMongoDocument, entryDescription->getTupleInstance().getTupleElementDataTypes().front());
		dataX.push_back(parameterValue);
	}
	
	_plotCfg.setUnitLabelX(createUnitLabel(entryDescription->getTupleInstance().getTupleUnits().front()));
	_plotCfg.setDataLabelX(entryDescription->getLabel());

	_plotCfg.setUnitLabelY(createUnitLabel(quantityInformation.getTupleInstance().getTupleUnits().front()));
	_plotCfg.setDataLabelY(quantityInformation.getLabel());

	const std::string curveNameBase = ot::EntityName::getSubName(_curveCfg.getEntityName()).value();
	std::list<ot::PlotDataset*> allCurves;
	
	bool showEntireMatrix = _plotCfg.getShowEntireMatrix();
	int32_t showMatrixRowEntry = _plotCfg.getShowMatrixRowEntry() - 1;
	int32_t showMatrixColumnEntry = _plotCfg.getShowMatrixColumnEntry() - 1;

	auto yIterator = dataY.begin();
	for (uint32_t j = 0; j < numberOfQuantityEntries; j++)
	{
		OTAssert(yIterator != dataY.end(), "DataY should have an entry for each quantity entry");

		ot::Plot1DCurveCfg newCurveCfg = _curveCfg;
		bool datasetHasSingleDatapoint = dataX.size() == 1;
		colourSetting.setPainter(newCurveCfg, j, datasetHasSingleDatapoint);
		
		if (numberOfQuantityEntries == 1)
		{
			ot::PlotDatasetData curveData = createCurveData(dataX, yIterator->first, yIterator->second, _plotCfg);
			ot::PlotDataset* singleCurve = new ot::PlotDataset(nullptr, newCurveCfg, std::move(curveData));
			singleCurve->setCurveNameBase(curveNameBase);
			allCurves.push_back(singleCurve);
		}
		else if(quantityDimensions.size() == 1) // Vector
		{
			newCurveCfg.setTitle(curveNameBase + " (" + std::to_string(j + 1) + ")");
			
			ot::PlotDatasetData curveData = createCurveData(dataX, yIterator->first, yIterator->second, _plotCfg);
			ot::PlotDataset* familyOfCurves= new ot::PlotDataset(nullptr, newCurveCfg, std::move(curveData));

			familyOfCurves->setConfig(_curveCfg);
			familyOfCurves->setCurveNameBase(curveNameBase);
			allCurves.push_back(familyOfCurves);
		}
		else if (quantityDimensions.size() == 2)
		{
			// Assumption csr matrix:
			uint32_t column = j / quantityDimensions[1];
			uint32_t row = j - column * quantityDimensions[1];
			if (showEntireMatrix || showMatrixColumnEntry == column && showMatrixRowEntry == row)
			{
				ot::PlotDatasetData curveData = createCurveData(dataX, yIterator->first, yIterator->second, _plotCfg);
				ot::PlotDataset* familyOfCurves = new ot::PlotDataset(nullptr, newCurveCfg, std::move(curveData));

				newCurveCfg.setTitle(curveNameBase + " (" + std::to_string(row + 1) + "," + std::to_string(column + 1) + ")");

				familyOfCurves->setConfig(newCurveCfg);
				familyOfCurves->setCurveNameBase(curveNameBase);
				allCurves.push_back(familyOfCurves);
			}
		}
		else
		{
			throw std::exception("Tensors not supported in 1D plot yet");
		}

		yIterator++;
	}

	m_curveIDDescriptions.push_back("");
	return allCurves;
}

std::list<ot::PlotDataset*> CurveDatasetFactory::createCurveFamily(ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _curveCfg, const std::string& _xAxisParameter, ot::ConstJsonArray& _allMongoDBDocuments)
{
	std::list<ot::PlotDataset*> dataSets;
		
	//Lables of curves will be <parameter_Label_1>_<parameter_Value_1>_<parameter_Unit_1>_ ... _<parameter_Label_n>_<parameter_Value_n>_<parameter_Unit_n>
	const ot::QueryInformation queryInformation = _curveCfg.getQueryInformation();
	const ot::DataPointDecoder& quantityInformation = queryInformation.getQuantityDescription();

	size_t numberOfDocuments = _allMongoDBDocuments.Size();
	std::string xAxisParameterLabel = _xAxisParameter;
	//Auto selection
	if (xAxisParameterLabel == "")
	{
		xAxisParameterLabel = queryInformation.getParameterDescriptions().begin()->getLabel();
	}

	const ot::DataPointDecoder* xAxisParameter = nullptr;
	for (const auto& parameterDescription : queryInformation.getParameterDescriptions())
	{
		if (parameterDescription.getLabel() == xAxisParameterLabel)
		{
			xAxisParameter = &parameterDescription;
		}
	}
	assert(xAxisParameter != nullptr); 


	std::map<std::string, std::list<Datapoints>> familyOfCurves;
	std::map<std::string, std::list<ShortParameterDescription>> additionalParameterDescByCurveName;
	std::map<std::string, std::list<std::string>> parameterValuesByParameterName;

	for (uint32_t i = 0; i < numberOfDocuments; i++) {
		auto singleMongoDocument = ot::json::getObject(_allMongoDBDocuments, i);

		//First build a unique name of the additional parameter values
		std::string curveName("");
		std::list<ShortParameterDescription> additionalParameterInfos;
		
		//Get values for the additional parameter and store their information
		for (const auto& additionalParameter :queryInformation.getParameterDescriptions())
		{
			if (&additionalParameter != xAxisParameter)
			{
				const ot::TupleInstance& parameterTuple =  additionalParameter.getTupleInstance();

				auto& additionalParameterEntry = singleMongoDocument[additionalParameter.getFieldName().c_str()];
				const std::string value =	ot::json::toJson(additionalParameterEntry);
				curveName += " (" + additionalParameter.getLabel() + "=" + value + " " + parameterTuple.getTupleUnits().front() + "); ";
				parameterValuesByParameterName[additionalParameter.getLabel()].push_back(value);
				
				ShortParameterDescription additionalParameterInfo;
				additionalParameterInfo.m_label = additionalParameter.getLabel();
				additionalParameterInfo.m_value = value;
				additionalParameterInfo.m_unit = parameterTuple.getTupleUnits().front();
				additionalParameterInfos.push_back(additionalParameterInfo);
			}
		}
		curveName = curveName.substr(0, curveName.size() - 2);

		//With a unique name, depending on the values of the additional parameter we now look if there is already data stored for this curve
		auto curve = familyOfCurves.find(curveName);
		if (curve == familyOfCurves.end()) 
		{
			Datapoints dataPoints;
			dataPoints.reserve(numberOfDocuments - i);
			familyOfCurves[curveName].push_back(std::move(dataPoints));
			additionalParameterDescByCurveName[curveName] = additionalParameterInfos;
			curve = familyOfCurves.find(curveName);
		}

		//Get quantity value
		std::list<Datapoints>& dataPoints = curve->second;
		if (dataPoints.size() == 0)
		{
			dataPoints.push_back(Datapoints());
		}
		const ot::TupleInstance& tupleInstance =	quantityInformation.getTupleInstance();
		if (ot::json::isArray(singleMongoDocument, quantityInformation.getFieldName()))
		{
			//Get x-axis value
			const double xAxisParameterValue = jsonToDouble(xAxisParameter->getFieldName(), singleMongoDocument, xAxisParameter->getTupleInstance().getTupleElementDataTypes().front());
			
			auto dataVector = ot::json::getArray(singleMongoDocument, quantityInformation.getFieldName());
			auto nextDatapointsContainer = dataPoints.begin();
			for (auto& data : dataVector)
			{
				if (nextDatapointsContainer == dataPoints.end())
				{
					dataPoints.push_back(Datapoints());
					nextDatapointsContainer = std::prev(dataPoints.end());
				}
				const double quantityValue = jsonToDouble(data, tupleInstance.getTupleElementDataTypes().front());
				
				nextDatapointsContainer->m_yData.push_back(quantityValue);
				nextDatapointsContainer->m_xData.push_back(xAxisParameterValue);

				nextDatapointsContainer++;
			}
		}
		else
		{
			
			const double quantityValue = jsonToDouble(quantityInformation.getFieldName(), singleMongoDocument, tupleInstance.getTupleElementDataTypes().front());
			dataPoints.front().m_yData.push_back(quantityValue);

			// Get x-axis value
			const double xAxisParameterValue = jsonToDouble(xAxisParameter->getFieldName(), singleMongoDocument, tupleInstance.getTupleElementDataTypes().front());
			dataPoints.front().m_xData.push_back(xAxisParameterValue);
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
	
	
	// Cases for additional parameter (x-axis parameter excluded):
	// 1) Only 1 additional Parameter -> Family of Curves (FoC): name of each curve shows the value of the additional parameter 
	//  1b) The additional parameter is constant -> Single curve: name of curve shows the value of the additional parameter
	// 2) More then 2 parameter -> FoC: name of each curve is abstracted to make it easier to read. The parameter values of each curve are communicated
	//  2b) All parameter are constant -> Single curve: name of curve is abstracted to make it easier to read. The parameter values of each curve are communicated
	//  2c) All but one parameter are constant -> FoC: name of each curve shows the value of the not-constant parameter. The other parameter values of each curve are communicated

	
	// In this case we need to make the names better readable. Since we have more then one parameter in the name 	
	size_t numberOfParameter =	queryInformation.getParameterDescriptions().size();
	const std::string simpleNameBase = _curveCfg.getTitle();

	if (numberOfParameter > 2)
	{	
		// Naming case 1b and 2c
		std::list<std::string> nonConstParameter;
		for (auto& parameterValues : parameterValuesByParameterName)
		{
			parameterValues.second.sort();
			parameterValues.second.unique();
			if (parameterValues.second.size() != 1)
			{
				nonConstParameter.push_back(parameterValues.first) ;
			}
		}
		std::map<std::string,std::list<Datapoints>> familyOfCurvesSimplerNames;
	
		if (nonConstParameter.size() == 1)
		{
			const std::string notConstParameterName = *nonConstParameter.begin();
			std::string message = simpleNameBase + ":\n";
			bool firstParameterDescription = true;
			for (auto& curve : familyOfCurves) {
				std::string complexName = curve.first;
				std::list<ShortParameterDescription>& additionalParameterDescription = additionalParameterDescByCurveName[curve.first];
				std::string simpleName = simpleNameBase + " (" + notConstParameterName;
				for (const ShortParameterDescription& description : additionalParameterDescription)
				{
					if (description.m_label == notConstParameterName)
					{
						simpleName += "=" + description.m_value + " " + description.m_unit + ")";
					}
					else if(firstParameterDescription)
					{
						message +=
							"	" + description.m_label + " = " + description.m_value + " " + description.m_unit + "\n";
						firstParameterDescription = false;
					}
				}

				familyOfCurvesSimplerNames.insert({ simpleName,std::move(curve.second) });
				curve.second = std::list<Datapoints>();
			}
			m_curveIDDescriptions.push_back(message);

		}
		else if (nonConstParameter.size() == 0)
		{
			// If all but the x-axis parameter are constant, we are having a single curve
			assert(familyOfCurves.size() == 1); 
			auto singleCurve = familyOfCurves.begin();

			familyOfCurvesSimplerNames.insert({ simpleNameBase,std::move(singleCurve->second)});
			
			std::list<ShortParameterDescription>& additionalParameterDescription = additionalParameterDescByCurveName[singleCurve->first];
			std::string message = simpleNameBase +":\n";
			for (auto entry : additionalParameterDescription) 
			{
				message += "	" + entry.m_label + " = " + entry.m_value + " " + entry.m_unit + "\n";
			}
			m_curveIDDescriptions.push_back(message);
		}
		else
		{
			std::list<std::string> runIDDescriptions;

			// The simpler name is build as Curve + ID
			int counter(1);
			double temp = std::log10(familyOfCurves.size());
			uint32_t numberOfDigits = static_cast<uint32_t>(std::ceil(temp));
			for (auto& curve : familyOfCurves) {

				std::string curveNumber = ot::String::fillPrefix(std::to_string(counter), numberOfDigits, '0');
				const std::string simpleName = simpleNameBase + " (curve " + curveNumber + ")";
				counter++;

				familyOfCurvesSimplerNames.insert({ simpleName,std::move(curve.second) });
				curve.second = std::list<Datapoints>();
				std::list<ShortParameterDescription>& additionalParameterDescription = additionalParameterDescByCurveName[curve.first];
				std::string message =
					simpleName + ":\n";
				for (auto entry : additionalParameterDescription) {
					message += "	" + entry.m_label + " = " + entry.m_value + " " + entry.m_unit + "\n";
				}
				m_curveIDDescriptions.push_back(message);
			}
		}
		
		familyOfCurves = std::move(familyOfCurvesSimplerNames);
	}
	
	
	
	// We may need to iterate the rainbow painter
	CurveColourSetter curveColourSetter(_curveCfg);
	for (auto& singleCurve : familyOfCurves) 
	{
		ot::Plot1DCurveCfg newCurveCfg = _curveCfg;
		
		std::string curveTitle = "";
		if (numberOfParameter == 2)
		{
			curveTitle = (simpleNameBase + " " + singleCurve.first);
		}
		else
		{
			curveTitle = (singleCurve.first);
		}

		if (singleCurve.second.size() == 1)
		{
			Datapoints& curveData =	*singleCurve.second.begin();
			newCurveCfg.setTitle(curveTitle);

			bool datasetHasSingleDatapoint = curveData.m_xData.size() == 1;
			curveColourSetter.setPainter(newCurveCfg, datasetHasSingleDatapoint);
			
			ot::PlotDatasetData datasetData(std::move(curveData.m_xData), std::move(curveData.m_yData));
			auto dataset = new ot::PlotDataset (nullptr, newCurveCfg, std::move(datasetData));
			dataset->setCurveNameBase(simpleNameBase);
			dataSets.push_back(dataset);
		}
		else
		{
			// Matrices

			std::list<Datapoints>& curveDataList = singleCurve.second;
			const std::vector<uint32_t>& quantityDimensions = queryInformation.getQuantityDescription().getDimension();
			uint32_t j(0);
			bool showEntireMatrix = _plotCfg.getShowEntireMatrix();
			int32_t showMatrixRowEntry = _plotCfg.getShowMatrixRowEntry() - 1;
			int32_t showMatrixColumnEntry = _plotCfg.getShowMatrixColumnEntry() - 1;

			for (Datapoints& curveData : curveDataList)
			{
				uint32_t row = j / quantityDimensions[1];
				uint32_t column = j - row * quantityDimensions[1];
				if (showEntireMatrix || showMatrixColumnEntry == column && showMatrixRowEntry == row)
				{
					
					const std::string curveTitleWithIndex = curveTitle + " (" + std::to_string(row + 1) + "," + std::to_string(column + 1) + ")";
					ot::Plot1DCurveCfg newCurveCfgSub = newCurveCfg;
					
					bool datasetHasSingleDatapoint = curveData.m_xData.size() == 1;
					curveColourSetter.setPainter(newCurveCfgSub, datasetHasSingleDatapoint);
				
					newCurveCfgSub.setTitle(curveTitleWithIndex);
					
					ot::PlotDatasetData datasetData(std::move(curveData.m_xData), std::move(curveData.m_yData));
					auto dataset = new ot::PlotDataset(nullptr, newCurveCfgSub, std::move(datasetData));
					dataset->setCurveNameBase(simpleNameBase);
					dataSets.push_back(dataset);
				}
				j++;

			}
		}	
	}
	
	_plotCfg.setUnitLabelX(createUnitLabel(xAxisParameter->getTupleInstance().getTupleUnits().front()));
	_plotCfg.setDataLabelX(xAxisParameter->getLabel());

	_plotCfg.setUnitLabelY(createUnitLabel(quantityInformation.getTupleInstance().getTupleUnits().front()));
	_plotCfg.setDataLabelY(quantityInformation.getLabel());

	return dataSets;
}

std::optional<ot::ValueComparisonDescription> CurveDatasetFactory::createValidValueComparison(const ot::DataPointDecoder& _desciption, const ot::ValueComparisonDescription& _comparison)
{
	const std::string fieldName = _comparison.getName();
	const std::string comparator = _comparison.getComparator();
	const std::string value = _comparison.getValue();
	if (!fieldName.empty() && !comparator.empty() && !value.empty() && comparator != " ")
	{
		ot::ValueComparisonDescription validComparison = _comparison;
		validComparison.setName(_desciption.getFieldName());
		ot::TupleInstance instance;
		instance.setTupleElementDataTypes({ _desciption.getTupleInstance().getTupleElementDataTypes().front() });
		validComparison.setTupleInstance(instance);
		return validComparison;
	}
	else
	{
		std::string reason("");
		if (fieldName.empty())
		{
			reason = "No name set.";
		}
		else if (comparator.empty() || comparator == " ")
		{
			reason = "No comparator selected.";
		}
		else
		{
			assert(value.empty());
			reason = "No value entered.";
		}
		const std::string skippedComparison = "Name: " + fieldName + " comparator: " + comparator + "value: " + value + " reason: " + reason;
		m_skippedValueComparisons.push_back(skippedComparison);
		return {};
	}
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
