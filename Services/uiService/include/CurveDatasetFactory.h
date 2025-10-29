// @otlicense

#pragma once
#include <string>
#include "ResultDataStorageAPI.h"
#include "OTWidgets/PlotDataset.h"
#include "OTCore/JSON.h"

class CurveDatasetFactory
{
public:
	CurveDatasetFactory(const std::string& _collectionName)
		: m_dataAccess(_collectionName){ }
	
	std::list<ot::PlotDataset*> createCurves(ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _curveCfg, const std::string& _xAxisParameter, const std::list<ValueComparisionDefinition>& _valueComparisions);

	const std::list<std::string>& getCurveIDDescriptions() { return m_curveIDDescriptions; }
private:

	enum class CurveType : uint32_t
	{
		m_single = 0,
		m_familyCurve = 1
	};

	std::list<std::string> m_curveIDDescriptions;
	std::list<std::string> m_skippedValueComparisions;
	DataStorageAPI::ResultDataStorageAPI m_dataAccess;

	std::string createAxisLabel(const std::string& _title, const std::string& _unit);

	ot::JsonDocument queryCurveData(const ot::QueryInformation& _queryInformation, const std::list<ValueComparisionDefinition>& _valueComparisions);
	const std::list<ValueComparisionDefinition> extractValidValueDescriptions(const ot::QueryInformation& _queryInformation, const std::list<ValueComparisionDefinition>& _valueComparisions);
	CurveType determineCurveType(const ot::QueryInformation& _curveCfg);
	std::list <ot::PlotDataset*> createSingleCurve(ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _curveCfg, ot::ConstJsonArray& _allMongoDBDocuments);
	std::list<ot::PlotDataset*> createCurveFamily(ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _curveCfg, const std::string& _xAxisParameter, ot::ConstJsonArray& _allMongoDBDocuments);
	
	std::optional<ValueComparisionDefinition> createValidValueComparision(const ot::QuantityContainerEntryDescription& _desciption, const ValueComparisionDefinition& _comparision);

	double jsonToDouble(const std::string& _memberName, ot::ConstJsonObject& _jesonEntry, const std::string& _dataType);
	double jsonToDouble(const rapidjson::Value& _jesonEntry, const std::string& _dataType);
};
