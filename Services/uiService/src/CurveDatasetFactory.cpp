#include "CurveDatasetFactory.h"
#include "ContainerFlexibleOwnership.h"
#include "OTCore/TypeNames.h"
#include "OTGui/StyleRefPainter2D.h"
//#include "ResultDataAccess/AdvancedQueryBuilder.h"
#include "OTCore/String.h"
std::list<ot::PlotDataset*> CurveDatasetFactory::createCurves(ot::Plot1DCurveCfg& _config, const std::string& _xAxisParameter, const std::list<ValueComparisionDefinition>& _queries)
{
	m_curveIDDescriptions.clear();
	auto queryInformation = _config.getQueryInformation();

	ot::JsonDocument entireResult = queryCurveData(queryInformation, _queries);
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

ot::JsonDocument CurveDatasetFactory::queryCurveData(const ot::QueryInformation& _queryInformation, const std::list<ValueComparisionDefinition>& _queries)
{
	for (const ValueComparisionDefinition& queryDefinition : _queries)
	{
		for (auto& parameterDescription : _queryInformation.m_parameterDescriptions)
		{
			if (parameterDescription.m_label == queryDefinition.getName())
			{
				const std::string fieldName = parameterDescription.m_fieldName;
				const std::string comparator = queryDefinition.getComparator();
				const std::string value = queryDefinition.getValue();

				break;
			}
		}
	}

	DataStorageAPI::DataStorageResponse dbResponse = m_dataAccess.SearchInResultCollection(_queryInformation.m_query, _queryInformation.m_projection, 0);
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
	ot::PlotDataset* singleCurve = new ot::PlotDataset(nullptr, _curveCfg, ot::PlotDatasetData(std::move(dataX),dataY.release()));
	return singleCurve;
}

struct Datapoints
{
	std::vector<double> m_xData;
	std::vector<double> m_yData;
	void reserve(size_t numberOfDataPoints)
	{
		m_xData.reserve(numberOfDataPoints);
		m_yData.reserve(numberOfDataPoints);
	}
	void shrinkToFit()
	{
		m_xData.shrink_to_fit();
		m_yData.shrink_to_fit();
	}
};

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


	//Should be selected via a property ToDo
	std::map<std::string, Datapoints> familyOfCurves;
	std::map<std::string, std::list<AdditionalParameterDescription>> additionalParameterDescByCurveName;
	for (uint32_t i = 0; i < numberOfDocuments; i++) {
		auto singleMongoDocument = ot::json::getObject(_allMongoDBDocuments, i);

		//First build a unique name of the additional parameter values
		std::string curveName("");
		std::list<AdditionalParameterDescription> additionalParameterInfos;
		
		//Currently the first parameter is considered for the x-axis
		for (auto additionalParameter = queryInformation.m_parameterDescriptions.begin(); additionalParameter != queryInformation.m_parameterDescriptions.end(); additionalParameter++) {
			if (&(*additionalParameter) != xAxisParameter)
			{
				auto& additionalParameterEntry = singleMongoDocument[additionalParameter->m_fieldName.c_str()];
				curveName += additionalParameter->m_label + "_" + ot::json::toJson(additionalParameterEntry) + "_" + additionalParameter->m_unit + "; ";

				AdditionalParameterDescription additionalParameterInfo;
				additionalParameterInfo.m_label = additionalParameter->m_label;
				additionalParameterInfo.m_value = ot::json::toJson(additionalParameterEntry);
				additionalParameterInfo.m_unit = additionalParameter->m_unit;
				additionalParameterInfos.push_back(additionalParameterInfo);
			}
		}
		curveName = curveName.substr(0, curveName.size() - 2);

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
		const double xAxisParameterValue = jsonToDouble(xAxisParameter->m_fieldName, singleMongoDocument, xAxisParameter->m_dataType);
		(curve->second).m_xData.push_back(xAxisParameterValue);
	}


	for (auto& curve : familyOfCurves)
	{
		curve.second.shrinkToFit();
	}

	//In this case we need to make the names better readable. Since we have more then one parameter in the name 
	size_t numberOfParameter =	queryInformation.m_parameterDescriptions.size();
	if (numberOfParameter > 2) {
		std::map<std::string, Datapoints> familyOfCurvesSimplerNames;
		std::list <std::string> runIDDescriptions;

		int counter(1);
		uint32_t numberOfDigits = static_cast<uint32_t>(std::floor(familyOfCurves.size() % 10));
		for (auto& curve : familyOfCurves) {

			std::string curveNumber = ot::String::fillPrefix(std::to_string(counter), numberOfDigits, '0');
			const std::string simpleName = "Curve_" + curveNumber;
			counter++;

			familyOfCurvesSimplerNames.insert({ simpleName,std::move(curve.second) });
			curve.second = Datapoints();
			std::list<AdditionalParameterDescription>& additionalParameterDescription = additionalParameterDescByCurveName[curve.first];
			std::string message =
				simpleName + ":\n";
			for (auto entry : additionalParameterDescription) {
				message += "	" + entry.m_label + " = " + entry.m_value + " " + entry.m_unit + "\n";
			}
			m_curveIDDescriptions.push_back(message);
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

		newCurveCfg.setTitle(singleCurve.first);
		newCurveCfg.setXAxisUnit(xAxisParameter->m_unit);
		newCurveCfg.setXAxisTitle(xAxisParameter->m_label);
		//singleCurveCfg.setXAxisTitle();
		//singleCurveCfg.setXAxisUnit();

		// Probably iterate the colour of each curve here
		
		std::unique_ptr<ot::ComplexNumberContainerCartesian> yData(new ot::ComplexNumberContainerCartesian());
		yData->m_real = std::move(singleCurve.second.m_yData);

		ot::PlotDatasetData datasetData(std::move(singleCurve.second.m_xData), yData.release());
		auto dataset = new ot::PlotDataset (nullptr, newCurveCfg, std::move(datasetData));
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
		throw std::invalid_argument("Curve data has not supported type: "+ _dataType);
	}

	return value;
}
