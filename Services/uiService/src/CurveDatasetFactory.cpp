// @otlicense

#include "CurveDatasetFactory.h"
#include "ContainerFlexibleOwnership.h"
#include "OTCore/TypeNames.h"
#include "OTGui/StyleRefPainter2D.h"
#include "AdvancedQueryBuilder.h"
#include "OTCore/ExplicitStringValueConverter.h"
#include "OTCore/String.h"
#include "bsoncxx/json.hpp"
#include "Datapoints.h"
#include "ShortParameterDescription.h"
#include "AppBase.h"
#include "OTCore/EntityName.h"
#include "OTGui/PainterRainbowIterator.h"
#include "CurveColourSetter.h"

std::list<ot::PlotDataset*> CurveDatasetFactory::createCurves(ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _config, const std::string& _xAxisParameter, const std::list<ValueComparisionDefinition>& _valueComparisions)
{
	m_curveIDDescriptions.clear();
	auto queryInformation = _config.getQueryInformation();

	ot::JsonDocument entireResult = queryCurveData(queryInformation, _valueComparisions);
	ot::ConstJsonArray allMongoDocuments = ot::json::getArray(entireResult, "Documents");
	if (allMongoDocuments.Size() == 0)
	{
		AppBase::instance()->appendInfoMessage(QString(("Curve " + _config.getTitle() + " cannot be visualised since the query returned no data.\n ").c_str()));
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

std::string CurveDatasetFactory::createAxisLabel(const std::string& _title, const std::string& _unit)
{
	return _title + " [" + _unit +"]";
}

ot::JsonDocument CurveDatasetFactory::queryCurveData(const ot::QueryInformation& _queryInformation, const std::list<ValueComparisionDefinition>& _valueComparisions)
{
	
	//First we find the valid value comparisions give them additional information from the query information
	std::list<ValueComparisionDefinition> validQueries = extractValidValueDescriptions(_queryInformation, _valueComparisions);

	//Now we build the queries from the set properties
	AdvancedQueryBuilder queryBuilder;
	std::list<BsonViewOrValue> additionalComparisions;
	for (ValueComparisionDefinition& valueComparision : validQueries)
	{
		const std::string type = valueComparision.getType();
		const std::string value = valueComparision.getValue();
		BsonViewOrValue comparision = queryBuilder.createComparison(valueComparision);
		additionalComparisions.push_back(std::move(comparision));
	}

	//Now we assemble the final query
	bsoncxx::document::value docProjection = bsoncxx::from_json(_queryInformation.m_projection);
	bsoncxx::document::view_or_value projection(docProjection);

	bsoncxx::document::value  docQuery = bsoncxx::from_json(_queryInformation.m_query);
	bsoncxx::document::view_or_value query (docQuery);
	if (additionalComparisions.size() > 0)
	{
		additionalComparisions.push_back(query);
		query = queryBuilder.connectWithAND(std::move(additionalComparisions));
	}
	const std::string temp = bsoncxx::to_json(query);

	DataStorageAPI::DataStorageResponse dbResponse = m_dataAccess.searchInResultCollection(query, projection, 0);
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

const std::list<ValueComparisionDefinition> CurveDatasetFactory::extractValidValueDescriptions(const ot::QueryInformation& _queryInformation, const std::list<ValueComparisionDefinition>& _valueComparisions)
{
	std::list<ValueComparisionDefinition> validValueDescriptions;
	for (const ValueComparisionDefinition& queryDefinition : _valueComparisions)
	{
		if (_queryInformation.m_quantityDescription.m_label == queryDefinition.getName())
		{
			auto& quantityDescription = _queryInformation.m_quantityDescription;
			auto validValueComparision = createValidValueComparision(quantityDescription, queryDefinition);
			if (validValueComparision.has_value())
			{
				validValueDescriptions.push_back(validValueComparision.value());
			}
		}
		else
		{
			for (auto& parameterDescription : _queryInformation.m_parameterDescriptions)
			{
				if (parameterDescription.m_label == queryDefinition.getName())
				{
					auto validValueComparision = createValidValueComparision(parameterDescription, queryDefinition);
					if (validValueComparision.has_value())
					{
						validValueDescriptions.push_back(validValueComparision.value());
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
	size_t numberOfParameter = _queryInformation.m_parameterDescriptions.size();
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
	const ot::QuantityContainerEntryDescription& quantityInformation = queryInformation.m_quantityDescription;
	assert(queryInformation.m_parameterDescriptions.size() == 1); // For a single curve there should be only one parameter

	std::vector<uint32_t> quantityDimensions =	quantityInformation.m_dimension;
	uint32_t numberOfQuantityEntries(1);
	for (uint32_t quantityDimension : quantityDimensions)
	{
		numberOfQuantityEntries *= quantityDimension;
	}

	//We may need to iterate the rainbow painter
	CurveColourSetter colourSetting(_curveCfg, numberOfQuantityEntries);
	
	std::vector<std::unique_ptr<ot::ComplexNumberContainerCartesian>> dataY; 
	dataY.resize(numberOfQuantityEntries);
	for (size_t i = 0; i < numberOfQuantityEntries; i++)
	{
		dataY[i].reset(new ot::ComplexNumberContainerCartesian());
		dataY[i]->m_real.reserve(numberOfDocuments);
	}
	
	
	std::vector<double>dataX;
	dataX.reserve(numberOfDocuments);

	auto entryDescription = queryInformation.m_parameterDescriptions.begin();

	for (uint32_t i = 0; i < numberOfDocuments; i++) {
		auto singleMongoDocument = ot::json::getObject(_allMongoDBDocuments, i);		

		//Get quantity value
		if (numberOfQuantityEntries == 1)
		{
			const double quantityValue = jsonToDouble(quantityInformation.m_fieldName, singleMongoDocument, quantityInformation.m_dataType);
			dataY[0]->m_real.push_back(quantityValue);
		}
		else
		{
			ot::ConstJsonArray matrix = ot::json::getArray(singleMongoDocument, quantityInformation.m_fieldName);
			assert(matrix.Size() == numberOfQuantityEntries);
			for (uint32_t j = 0; j < numberOfQuantityEntries; j++)
			{
				const rapidjson::Value& matrixEntry = matrix[j];
				const double quantityValue = jsonToDouble(matrixEntry, quantityInformation.m_dataType);
				dataY[j]->m_real.push_back(quantityValue);
			}
		}
		//Get parameter value
		
		double parameterValue = jsonToDouble(entryDescription->m_fieldName, singleMongoDocument, entryDescription->m_dataType);
		dataX.push_back(parameterValue);
	}
	
	if (_plotCfg.getXLabelAxisAutoDetermine())
	{
		_plotCfg.setAxisLabelX(createAxisLabel(entryDescription->m_label, entryDescription->m_unit));
	}
	if (_plotCfg.getYLabelAxisAutoDetermine())
	{
		_plotCfg.setAxisLabelY(createAxisLabel(quantityInformation.m_label, quantityInformation.m_unit));
	}
	
	const std::string curveNameBase = ot::EntityName::getSubName(_curveCfg.getEntityName()).value();
	std::list<ot::PlotDataset*> allCurves;
	
	bool showEntireMatrix = _plotCfg.getShowEntireMatrix();
	int32_t showMatrixRowEntry = _plotCfg.getShowMatrixRowEntry() - 1;
	int32_t showMatrixColumnEntry = _plotCfg.getShowMatrixColumnEntry() - 1;


	for (uint32_t j = 0; j < numberOfQuantityEntries; j++)
	{
		auto newCurveCfg = _curveCfg;
		bool datasetHasSingleDatapoint = dataX.size() == 1;
		colourSetting.setPainter(newCurveCfg, j, datasetHasSingleDatapoint);
		
		if (numberOfQuantityEntries == 1)
		{

			ot::PlotDataset* singleCurve = new ot::PlotDataset(nullptr, newCurveCfg, ot::PlotDatasetData(dataX, dataY[j].release()));
			singleCurve->setCurveNameBase(curveNameBase);
			allCurves.push_back(singleCurve);
		}
		else if(quantityDimensions.size() == 1) // Vector
		{
			newCurveCfg.setTitle(curveNameBase + " (" + std::to_string(j + 1) + ")");
			
			ot::PlotDataset* familyOfCurves= new ot::PlotDataset(nullptr, newCurveCfg, ot::PlotDatasetData(dataX, dataY[j].release()));

			familyOfCurves->setConfig(_curveCfg);
			familyOfCurves->setCurveNameBase(curveNameBase);
			allCurves.push_back(familyOfCurves);
		}
		else if (quantityDimensions.size() == 2)
		{
			//Assumption csr matrix:
			uint32_t column = j/ quantityDimensions[1];
			uint32_t row = j - column * quantityDimensions[1];
			if (showEntireMatrix || showMatrixColumnEntry == column && showMatrixRowEntry == row)
			{
				ot::PlotDataset* familyOfCurves = new ot::PlotDataset(nullptr, newCurveCfg, ot::PlotDatasetData(dataX, dataY[j].release()));

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
	}

	m_curveIDDescriptions.push_back("");
	return allCurves;
}

std::list<ot::PlotDataset*> CurveDatasetFactory::createCurveFamily(ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _curveCfg, const std::string& _xAxisParameter, ot::ConstJsonArray& _allMongoDBDocuments)
{
	std::list<ot::PlotDataset*> dataSets;
		
	//Lables of curves will be <parameter_Label_1>_<parameter_Value_1>_<parameter_Unit_1>_ ... _<parameter_Label_n>_<parameter_Value_n>_<parameter_Unit_n>
	const ot::QueryInformation queryInformation = _curveCfg.getQueryInformation();
	const ot::QuantityContainerEntryDescription& quantityInformation = queryInformation.m_quantityDescription;

	size_t numberOfDocuments = _allMongoDBDocuments.Size();
	std::string xAxisParameterLabel = _xAxisParameter;
	//Auto selection
	if (xAxisParameterLabel == "")
	{
		xAxisParameterLabel = queryInformation.m_parameterDescriptions.begin()->m_label;
	}

	const ot::QuantityContainerEntryDescription* xAxisParameter = nullptr;
	for (auto& parameterDescription : queryInformation.m_parameterDescriptions)
	{
		if (parameterDescription.m_label == xAxisParameterLabel)
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
		for (auto& additionalParameter :queryInformation.m_parameterDescriptions) 
		{
			if (&additionalParameter != xAxisParameter)
			{
				auto& additionalParameterEntry = singleMongoDocument[additionalParameter.m_fieldName.c_str()];
				const std::string value =	ot::json::toJson(additionalParameterEntry);
				curveName += " (" + additionalParameter.m_label + "=" + value + " " + additionalParameter.m_unit + "); ";
				parameterValuesByParameterName[additionalParameter.m_label].push_back(value);
				
				ShortParameterDescription additionalParameterInfo;
				additionalParameterInfo.m_label = additionalParameter.m_label;
				additionalParameterInfo.m_value = value;
				additionalParameterInfo.m_unit = additionalParameter.m_unit;
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

		if (ot::json::isArray(singleMongoDocument, quantityInformation.m_fieldName))
		{
			//Get x-axis value
			const double xAxisParameterValue = jsonToDouble(xAxisParameter->m_fieldName, singleMongoDocument, xAxisParameter->m_dataType);
			
			auto dataVector = ot::json::getArray(singleMongoDocument, quantityInformation.m_fieldName);
			auto nextDatapointsContainer = dataPoints.begin();
			for (auto& data : dataVector)
			{
				if (nextDatapointsContainer == dataPoints.end())
				{
					dataPoints.push_back(Datapoints());
					nextDatapointsContainer = std::prev(dataPoints.end());
				}
				const double quantityValue = jsonToDouble(data, quantityInformation.m_dataType);
				
				nextDatapointsContainer->m_yData.push_back(quantityValue);
				nextDatapointsContainer->m_xData.push_back(xAxisParameterValue);

				nextDatapointsContainer++;
			}
		}
		else
		{
			
			const double quantityValue = jsonToDouble(quantityInformation.m_fieldName, singleMongoDocument, quantityInformation.m_dataType);
			dataPoints.front().m_yData.push_back(quantityValue);

			//Get x-axis value
			const double xAxisParameterValue = jsonToDouble(xAxisParameter->m_fieldName, singleMongoDocument, xAxisParameter->m_dataType);
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
	
	
	//Cases for additional parameter (x-axis parameter excluded):
	//1) Only 1 additional Parameter -> Family of Curves (FoC): name of each curve shows the value of the additional parameter 
	// 1b) The additional parameter is constant -> Single curve: name of curve shows the value of the additional parameter
	//2) More then 2 parameter -> FoC: name of each curve is abstracted to make it easier to read. The parameter values of each curve are communicated
	// 2b) All parameter are constant -> Single curve: name of curve is abstracted to make it easier to read. The parameter values of each curve are communicated
	// 2c) All but one parameter are constant -> FoC: name of each curve shows the value of the not-constant parameter. The other parameter values of each curve are communicated

	
	//In this case we need to make the names better readable. Since we have more then one parameter in the name 	
	size_t numberOfParameter =	queryInformation.m_parameterDescriptions.size();
	const std::string simpleNameBase = _curveCfg.getTitle();

	if (numberOfParameter > 2)
	{	
		//Naming case 1b and 2c
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
			//If all but the x-axis parameter are constant, we are having a single curve
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
			std::list <std::string> runIDDescriptions;

			//The simpler name is build as Curve + ID
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
	
	
	
	//We may need to iterate the rainbow painter
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
			std::unique_ptr<ot::ComplexNumberContainerCartesian> yData(new ot::ComplexNumberContainerCartesian());
			yData->m_real = std::move(curveData.m_yData);

			bool datasetHasSingleDatapoint = curveData.m_xData.size() == 1;
			curveColourSetter.setPainter(newCurveCfg, datasetHasSingleDatapoint);
			
			ot::PlotDatasetData datasetData(std::move(curveData.m_xData), yData.release());
			auto dataset = new ot::PlotDataset (nullptr, newCurveCfg, std::move(datasetData));
			dataset->setCurveNameBase(simpleNameBase);
			dataSets.push_back(dataset);
		}
		else
		{
			//Matrices

			std::list<Datapoints>& curveDataList = singleCurve.second;
			const std::vector<uint32_t>& quantityDimensions = queryInformation.m_quantityDescription.m_dimension;
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
				
					newCurveCfg.setTitle(curveTitleWithIndex);
					std::unique_ptr<ot::ComplexNumberContainerCartesian> yData(new ot::ComplexNumberContainerCartesian());
					yData->m_real = std::move(curveData.m_yData);

					ot::PlotDatasetData datasetData(std::move(curveData.m_xData), yData.release());
					auto dataset = new ot::PlotDataset(nullptr, newCurveCfg, std::move(datasetData));
					dataset->setCurveNameBase(simpleNameBase);
					dataSets.push_back(dataset);
				}
				j++;

			}
		}	
	}


	if (_plotCfg.getYLabelAxisAutoDetermine())
	{
		_plotCfg.setAxisLabelY(createAxisLabel(quantityInformation.m_label, quantityInformation.m_unit));
	}
	if (_plotCfg.getXLabelAxisAutoDetermine())
	{
		_plotCfg.setAxisLabelX(createAxisLabel(xAxisParameter->m_label, xAxisParameter->m_unit));
	}

	return dataSets;
}

std::optional<ValueComparisionDefinition> CurveDatasetFactory::createValidValueComparision(const ot::QuantityContainerEntryDescription& _desciption, const ValueComparisionDefinition& _comparision)
{
	const std::string fieldName = _comparision.getName();
	const std::string comparator = _comparision.getComparator();
	const std::string value = _comparision.getValue();
	if (!fieldName.empty() && !comparator.empty() && !value.empty() && comparator != " ")
	{
		ValueComparisionDefinition validComparision = _comparision;
		validComparision.setName(_desciption.m_fieldName);
		validComparision.setType(_desciption.m_dataType);
		return validComparision;
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
		const std::string skippedComparision = "Name: " + fieldName + " comparator: " + comparator + "value: " + value + " reason: " + reason;
		m_skippedValueComparisions.push_back(skippedComparision);
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
		throw std::invalid_argument("Curve data has not supported type: "+ _dataType);
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
