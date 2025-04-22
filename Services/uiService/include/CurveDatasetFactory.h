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
	
	std::list<ot::PlotDataset*> createCurves(ot::Plot1DCurveCfg& _curveCfg, const std::string& _xAxisParameter, const std::list<ValueComparisionDefinition>& _queries);

	const std::vector<std::string>& getCurveIDDescriptions() { return m_curveIDDescriptions; }
private:
	struct AdditionalParameterDescription {
		std::string m_label = "";
		std::string m_unit = "";
		std::string m_value = "";
	};

	enum class CurveType : uint32_t
	{
		m_single = 0,
		m_familyCurve = 1
	};

	std::vector<std::string> m_curveIDDescriptions;

	DataStorageAPI::ResultDataStorageAPI m_dataAccess;

	ot::JsonDocument queryCurveData(const ot::QueryInformation& _queryInformation, const std::list<ValueComparisionDefinition>& _queries);
	CurveType determineCurveType(const ot::QueryInformation& _curveCfg);
	ot::PlotDataset* createSingleCurve(ot::Plot1DCurveCfg& _curveCfg, ot::ConstJsonArray& _allMongoDBDocuments);
	std::list<ot::PlotDataset*> createCurveFamily(ot::Plot1DCurveCfg& _curveCfg, const std::string& _xAxisParameter, ot::ConstJsonArray& _allMongoDBDocuments);
	
	std::optional<ValueComparisionDefinition> createValidValueComparision(const ot::QuantityContainerEntryDescription& _desciption, const ValueComparisionDefinition& _comparision);

	double jsonToDouble(const std::string& _memberName, ot::ConstJsonObject& _jesonEntry, const std::string& _dataType);
};
