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

std::list<ot::PlotDataset*> CurveDatasetFactory::createCurves(ot::Plot1DCurveCfg& _config, const std::string& _xAxisParameter, const std::list<ValueComparisionDefinition>& _valueComparisions)
{
	m_curveIDDescriptions.clear();
	auto queryInformation = _config.getQueryInformation();

	ot::JsonDocument entireResult = queryCurveData(queryInformation, _valueComparisions);
	ot::ConstJsonArray allMongoDocuments = ot::json::getArray(entireResult, "Documents");
	
	CurveType curveType = determineCurveType(queryInformation);
	
	std::list<ot::PlotDataset*> dataSets;
	if (curveType == CurveType::m_single)
	{
		ot::PlotDataset* dataset = createSingleCurve(_config, allMongoDocuments);
		dataSets.push_back(dataset);
	}
	else
	{
		assert(curveType == CurveType::m_familyCurve);
		dataSets = createCurveFamily(_config, _xAxisParameter, allMongoDocuments);
	}
	return dataSets;
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

	DataStorageAPI::DataStorageResponse dbResponse = m_dataAccess.SearchInResultCollection(query, projection, 0);
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

ot::PlotDataset* CurveDatasetFactory::createSingleCurve(ot::Plot1DCurveCfg& _curveCfg, ot::ConstJsonArray& _allMongoDBDocuments)
{
	const uint32_t numberOfDocuments = _allMongoDBDocuments.Size();
	const ot::QueryInformation& queryInformation =_curveCfg.getQueryInformation();
	const ot::QuantityContainerEntryDescription& quantityInformation = queryInformation.m_quantityDescription;
	assert(queryInformation.m_parameterDescriptions.size() == 1); // For a single curve there should be only one parameter

	std::unique_ptr<ot::ComplexNumberContainerCartesian> dataY(new ot::ComplexNumberContainerCartesian());
	std::vector<double>dataX;
	dataY->m_real.reserve(numberOfDocuments);
	dataX.reserve(numberOfDocuments);

	auto entryDescription = queryInformation.m_parameterDescriptions.begin();
	for (uint32_t i = 0; i < numberOfDocuments; i++) {
		auto singleMongoDocument = ot::json::getObject(_allMongoDBDocuments, i);		

		//Get quantity value
		const double quantityValue = jsonToDouble(quantityInformation.m_fieldName, singleMongoDocument, quantityInformation.m_dataType);
		dataY->m_real.push_back(quantityValue);
		
		//Get parameter value
		
		double parameterValue = jsonToDouble(entryDescription->m_fieldName, singleMongoDocument, entryDescription->m_dataType);
		dataX.push_back(parameterValue);
	}
	
	if (_curveCfg.getXAxisUnit().empty())
	{
		_curveCfg.setXAxisUnit(entryDescription->m_unit); //Could be overwritten be the calling code, the default is the parameter unit.
	}

	if (_curveCfg.getTitle().empty())
	{
		const std::string entityName = _curveCfg.getEntityName();
		const std::string shortName = entityName.substr(entityName.find_last_of("/") + 1);
		_curveCfg.setTitle(shortName);
	}

	ot::PlotDataset* singleCurve = new ot::PlotDataset(nullptr, _curveCfg, ot::PlotDatasetData(std::move(dataX),dataY.release()));
	return singleCurve;
}

std::list<ot::PlotDataset*> CurveDatasetFactory::createCurveFamily(ot::Plot1DCurveCfg& _curveCfg, const std::string& _xAxisParameter, ot::ConstJsonArray& _allMongoDBDocuments)
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


	std::map<std::string, Datapoints> familyOfCurves;
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
				curveName += additionalParameter.m_label + "_" + value + "_" + additionalParameter.m_unit + "; ";
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
			familyOfCurves.insert(std::make_pair(curveName, std::move(dataPoints)));
			additionalParameterDescByCurveName[curveName] = additionalParameterInfos;
			curve = familyOfCurves.find(curveName);
		}

		//Get quantity value
		const double quantityValue = jsonToDouble(quantityInformation.m_fieldName, singleMongoDocument, quantityInformation.m_dataType);
		(curve->second).m_yData.push_back(quantityValue);
		
		//Get x-axis value
		const double xAxisParameterValue = jsonToDouble(xAxisParameter->m_fieldName, singleMongoDocument, xAxisParameter->m_dataType);
		(curve->second).m_xData.push_back(xAxisParameterValue);
	}

	for (auto& curve : familyOfCurves)
	{
		curve.second.shrinkToFit();
	}
	
	
	//Cases for additional parameter (x-axis parameter excluded):
	//1) Only 1 additional Parameter -> Family of Curves (FoC): name of each curve shows the value of the additional parameter 
	// 1b) The additional parameter is constant -> Single curve: name of curve shows the value of the additional parameter
	//2) More then 2 parameter -> FoC: name of each curve is abstracted to make it easier to read. The parameter values of each curve are communicated
	// 2b) All parameter are constant -> Single curve: name of curve is abstracted to make it easier to read. The parameter values of each curve are communicated
	// 2c) All but one parameter are constant -> FoC: name of each curve shows the value of the not-constant parameter. The other parameter values of each curve are communicated

	
	//In this case we need to make the names better readable. Since we have more then one parameter in the name 

	const std::string entityName = _curveCfg.getEntityName();
	const std::string shortName = entityName.substr(entityName.find_last_of("/") + 1);

	size_t numberOfParameter =	queryInformation.m_parameterDescriptions.size();
	if (numberOfParameter > 2)
	{	
		//Naming case 1b and 2c
		std::list<std::string> nonConstParameter;
		for (auto& parameterValues : parameterValuesByParameterName)
		{
			parameterValues.second.unique();
			if (parameterValues.second.size() != 1)
			{
				nonConstParameter.push_back(parameterValues.first) ;
			}
		}
		std::map<std::string, Datapoints> familyOfCurvesSimplerNames;

		if (nonConstParameter.size() == 1)
		{
			const std::string notConstParameterName = *nonConstParameter.begin();
			for (auto& curve : familyOfCurves) {
				std::string complexName = curve.first;
				std::list<ShortParameterDescription>& additionalParameterDescription = additionalParameterDescByCurveName[curve.first];
				std::string simpleName = shortName + "_" + notConstParameterName;
				for (const ShortParameterDescription& description : additionalParameterDescription)
				{
					if (description.m_label == notConstParameterName)
					{
						simpleName += "_" + description.m_value + "_" + description.m_unit;
					}
				}

				familyOfCurvesSimplerNames.insert({ simpleName,std::move(curve.second) });
				curve.second = Datapoints();
			}
		}
		else if (nonConstParameter.size() == 0)
		{
			//If all but the x-axis parameter are constant, we are having a single curve
			assert(familyOfCurves.size() == 1); 
			auto singleCurve = familyOfCurves.begin();

			familyOfCurvesSimplerNames.insert({ shortName,std::move(singleCurve->second)});
			
			std::list<ShortParameterDescription>& additionalParameterDescription = additionalParameterDescByCurveName[singleCurve->first];
			std::string message = shortName +":\n";
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
				const std::string simpleName = shortName + "_curve_" + curveNumber;
				counter++;

				familyOfCurvesSimplerNames.insert({ simpleName,std::move(curve.second) });
				curve.second = Datapoints();
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
	
	uint32_t colourIndex = static_cast<uint32_t>(ot::ColorStyleValueEntry::RainbowFirst);
	
	for (auto& singleCurve : familyOfCurves) {
		ot::Plot1DCurveCfg newCurveCfg = _curveCfg;
		auto pen = newCurveCfg.getLinePen();

		ot::ColorStyleValueEntry styleEntry = static_cast<ot::ColorStyleValueEntry>(colourIndex);
		auto stylePainter = new ot::StyleRefPainter2D(styleEntry);
		newCurveCfg.setLinePen(stylePainter);
		
		if (colourIndex < static_cast<uint32_t>(ot::ColorStyleValueEntry::RainbowLast))
		{
			colourIndex++;
		}
		else
		{
			colourIndex = static_cast<uint32_t>(ot::ColorStyleValueEntry::RainbowFirst); //Restart the rainbow
		}

		if (numberOfParameter == 1)
		{
			newCurveCfg.setTitle(shortName + "_" + singleCurve.first);
		}
		else
		{
			newCurveCfg.setTitle(singleCurve.first);
		}
		newCurveCfg.setXAxisUnit(xAxisParameter->m_unit);
		newCurveCfg.setXAxisTitle(xAxisParameter->m_label);
				
		std::unique_ptr<ot::ComplexNumberContainerCartesian> yData(new ot::ComplexNumberContainerCartesian());
		yData->m_real = std::move(singleCurve.second.m_yData);

		ot::PlotDatasetData datasetData(std::move(singleCurve.second.m_xData), yData.release());
		auto dataset = new ot::PlotDataset (nullptr, newCurveCfg, std::move(datasetData));
		dataSets.push_back(dataset);
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
