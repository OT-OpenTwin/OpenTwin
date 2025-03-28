#include "CurveDatasetFactory.h"
#include "ContainerFlexibleOwnership.h"
#include "OTCore/TypeNames.h"

std::list<ot::PlotDataset*> CurveDatasetFactory::createCurves(ot::Plot1DCurveCfg& _config)
{
	auto queryInformation = _config.getQueryInformation();

	ot::ConstJsonArray allMongoDocuments = queryCurveData(queryInformation);
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
		dataSets = createCurveFamily(_config, allMongoDocuments);
	}
	return dataSets;
}

ot::ConstJsonArray CurveDatasetFactory::queryCurveData(const ot::QueryInformation& _queryInformation)
{
	DataStorageAPI::DataStorageResponse dbResponse = m_dataAccess.SearchInResultCollection(_queryInformation.m_query, _queryInformation.m_projection, 0);
	if (dbResponse.getSuccess()) 
	{
		const std::string queryResponse = dbResponse.getResult();
		ot::JsonDocument doc;
		doc.fromJson(queryResponse);
		ot::ConstJsonArray allMongoDocuments = ot::json::getArray(doc, "Documents");
		return allMongoDocuments;
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

	std::unique_ptr<double[]> dataY(new double[numberOfDocuments]);
	std::unique_ptr<double[]> dataX(new double[numberOfDocuments]);

	auto entryDescription = queryInformation.m_parameterDescriptions.begin();
	for (uint32_t i = 0; i < numberOfDocuments; i++) {
		auto singleMongoDocument = ot::json::getObject(_allMongoDBDocuments, i);		

		//Get quantity value
		const double quantityValue = jsonToDouble(quantityInformation.m_fieldName, singleMongoDocument, quantityInformation.m_dataType);
		(dataY)[i] = quantityValue;
		
		//Get parameter value
		
		double parameterValue = jsonToDouble(entryDescription->m_fieldName, singleMongoDocument, entryDescription->m_dataType);
		(dataX)[i] = static_cast<double>(parameterValue);
	}

	double* xData = dataX.release();
	double* yData = dataY.release();
	double* dataXIm(nullptr), * dataYIm(nullptr); //Currently no complex numbers
	
	if (_curveCfg.getXAxisUnit().empty())
	{
		_curveCfg.setXAxisUnit(entryDescription->m_unit); //Could be overwritten be the calling code, the default is the parameter unit.
	}
	ot::PlotDataset* singleCurve = new ot::PlotDataset(nullptr, _curveCfg, ot::PlotDatasetData(xData, dataXIm, yData, dataYIm, nullptr, numberOfDocuments));
	return singleCurve;
}

std::list<ot::PlotDataset*> CurveDatasetFactory::createCurveFamily(ot::Plot1DCurveCfg& _curveCfg, ot::ConstJsonArray& _allMongoDBDocuments)
{
	std::list<ot::PlotDataset*> dataSets;

	//Lables of curves will be <parameter_Label_1>_<parameter_Value_1>_<parameter_Unit_1>_ ... _<parameter_Label_n>_<parameter_Value_n>_<parameter_Unit_n>
	const ot::QueryInformation queryInformation = _curveCfg.getQueryInformation();
	const ot::QuantityContainerEntryDescription& quantityInformation = queryInformation.m_quantityDescription;

	size_t numberOfDocuments = _allMongoDBDocuments.Size();

	std::unique_ptr<double[]> dataY(new double[numberOfDocuments]);

	//Should be selected via a property ToDo
	auto xAxisParameter = queryInformation.m_parameterDescriptions.begin();
	std::map<std::string, ContainerFlexibleOwnership<double>> familyOfCurves;
	std::map<std::string, std::list<AdditionalParameterDescription>> additionalParameterDescByCurveName;
	for (uint32_t i = 0; i < numberOfDocuments; i++) {
		auto singleMongoDocument = ot::json::getObject(_allMongoDBDocuments, i);

		//Get quantity value
		const double quantityValue = jsonToDouble(quantityInformation.m_fieldName, singleMongoDocument, quantityInformation.m_dataType);
		(dataY)[i] = static_cast<double>(quantityValue);

		//First build a unique name of the additional parameter values
		std::string curveName("");
		std::list<AdditionalParameterDescription> additionalParameterInfos;
		for (auto additionalParameter = queryInformation.m_parameterDescriptions.begin()++; additionalParameter != queryInformation.m_parameterDescriptions.end(); additionalParameter++) {
			auto& additionalParameterEntry = singleMongoDocument[additionalParameter->m_fieldName.c_str()];
			curveName = additionalParameter->m_label + "_" + ot::json::toJson(additionalParameterEntry) + "_" + additionalParameter->m_unit + "; ";

			AdditionalParameterDescription additionalParameterInfo;
			additionalParameterInfo.m_label = additionalParameter->m_label;
			additionalParameterInfo.m_value = ot::json::toJson(additionalParameterEntry);
			additionalParameterInfo.m_unit = additionalParameter->m_unit;
			additionalParameterInfos.push_back(additionalParameterInfo);
		}
		curveName = curveName.substr(0, curveName.size() - 2);

		auto curve = familyOfCurves.find(curveName);
		if (curve == familyOfCurves.end()) {
			familyOfCurves.insert({ curveName, ContainerFlexibleOwnership<double>(numberOfDocuments) });
			additionalParameterDescByCurveName[curveName] = additionalParameterInfos;
			curve = familyOfCurves.find(curveName);
		}

		const double xAxisParameterValue = jsonToDouble(xAxisParameter->m_fieldName, singleMongoDocument, xAxisParameter->m_dataType);
		(curve->second).pushBack(xAxisParameterValue);
	}

	//In this case we need to make the names better readable. Since we have more then one parameter in the name 
	size_t numberOfParameter =	queryInformation.m_parameterDescriptions.size();
	if (numberOfParameter > 2) {
		std::map<std::string, ContainerFlexibleOwnership<double>> familyOfCurvesSimplerNames;
		std::list <std::string> runIDDescriptions;

		int counter(0);
		for (auto curve : familyOfCurves) {
			const std::string simpleName = "RunID_" + std::to_string(counter);
			familyOfCurvesSimplerNames.insert({ simpleName,curve.second });
			counter++;
			std::list<AdditionalParameterDescription>& additionalParameterDescription = additionalParameterDescByCurveName[curve.first];
			std::string message =
				simpleName + ":\n";
			for (auto entry : additionalParameterDescription) {
				message += "	" + entry.m_label + " = " + entry.m_value + " " + entry.m_unit + "\n";
			}
		}

		familyOfCurves = familyOfCurvesSimplerNames;
	}

	for (auto singleCurve : familyOfCurves) {
		ot::Plot1DCurveCfg singleCurveCfg = _curveCfg;
		//singleCurveCfg.setXAxisTitle();
		//singleCurveCfg.setXAxisUnit();


		// Probably iterate the colour of each curve here
		ot::Color colour = singleCurveCfg.getLinePenColor();

		double* xData = singleCurve.second.release();
		double* yData = dataY.release();
		/*
		std::string dbg = "Adding cuve with points:";
		for (size_t i = 0; i < numberOfDocuments; i++) {
			dbg.append("\n[" + std::to_string(i) + "]: " + std::to_string(xData[i]) + "; " + std::to_string(yData[i]));
		}
		dbg.append("\n\nn = " + std::to_string(numberOfDocuments) + "\n\n");

		OT_LOG_W(dbg);
		*/
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
