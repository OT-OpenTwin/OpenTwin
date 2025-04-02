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
	
	std::list<ot::PlotDataset*> createCurves(ot::Plot1DCurveCfg& _curveCfg);

	const std::vector<std::string>& getRunIDDescriptions() { return m_runIDDescriptions; }
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

	std::vector<std::string> m_runIDDescriptions;

	DataStorageAPI::ResultDataStorageAPI m_dataAccess;

	ot::JsonDocument queryCurveData(const ot::QueryInformation& _queryInformation);
	CurveType determineCurveType(const ot::QueryInformation& _curveCfg);
	ot::PlotDataset* createSingleCurve(ot::Plot1DCurveCfg& _curveCfg, ot::ConstJsonArray& _allMongoDBDocuments);
	std::list<ot::PlotDataset*> createCurveFamily(ot::Plot1DCurveCfg& _curveCfg, ot::ConstJsonArray& _allMongoDBDocuments);

	double jsonToDouble(const std::string& _memberName, ot::ConstJsonObject& _jesonEntry, const std::string& _dataType);
};
