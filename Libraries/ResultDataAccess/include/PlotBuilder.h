#pragma once
#include <string>

#include "DatasetDescription.h"
#include "OTGui/Plot1DCurveCfg.h"
#include "OTGui/Plot1DCfg.h"
#include "OTModelAPI/NewModelStateInformation.h"
#include "ResultCollectionExtender.h"

class __declspec(dllexport) PlotBuilder
{
public:
	PlotBuilder(ResultCollectionExtender& _extender, const std::string& _owner);
	void addCurve(DatasetDescription&& _dataSetDescription, ot::Plot1DCurveCfg& _config, const std::string& _seriesName);
	void addCurve(std::list<DatasetDescription>&& _dataSetDescriptions, ot::Plot1DCurveCfg& _config, const std::string& _seriesName);
	void buildPlot(ot::Plot1DCfg& _plotCfg, bool _saveModelState = true);

private:
	ot::NewModelStateInformation m_newModelStateInformation;
	const std::string m_owner;
	ResultCollectionExtender& m_extender;
	std::list<std::string> m_parameterLabels;
	std::list<std::string> m_quantityLabel;

	bool validityCheck(std::list<DatasetDescription>& _dataSetDescriptions, ot::Plot1DCurveCfg& _config);

	void storeCurve(std::list<DatasetDescription>&& _dataSetDescriptions, ot::Plot1DCurveCfg& _config, const std::string& _seriesName);
	
	const std::string createQuery(ot::UID _seriesID);
	const std::string createProjection();

	void createPlot(ot::Plot1DCfg& _plotCfg);
};
