#pragma once
#include <string>
#include "ResultDataStorageAPI.h"
#include "OTWidgets/PlotDataset.h"

class CurveDatasetFactory
{
public:
	CurveDatasetFactory(const std::string& _collectionName)
		: m_dataAccess(_collectionName){ }
	
	std::list<ot::PlotDataset*> createCurves(const ot::Plot1DCurveCfg& _curveCfg);

private:
	struct AdditionalParameterDescription {
		std::string m_label = "";
		std::string m_unit = "";
		std::string m_value = "";
	};

	DataStorageAPI::ResultDataStorageAPI m_dataAccess;
};
