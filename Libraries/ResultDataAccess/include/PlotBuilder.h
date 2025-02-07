#pragma once
#include <string>

#include "DatasetDescription.h"
#include "OTGui/Plot1DCurveInfoCfg.h"
#include "OTGui/Plot1DCfg.h"
#include "OTModelAPI/NewModelStateInformation.h"
#include "ResultCollectionExtender.h"

class PlotBuilder
{
public:
	PlotBuilder(ResultCollectionExtender& _extender, const std::string& _owner);
	void addCurve(DatasetDescription&& _dataSetDescription, ot::Plot1DCurveInfoCfg& _config);
	void buildPlot(const ot::Plot1DCfg& _plotCfg, bool _saveModelState = true);

private:
	ot::NewModelStateInformation m_newModelStateInformation;
	const std::string m_owner;
	ResultCollectionExtender& m_extender;

	void storeCurve(DatasetDescription&& _dataSetDescription, ot::Plot1DCurveInfoCfg& _config);
	
	const std::string createQuery(ot::UID _seriesID);
	const std::string createProjection();

	void createPlot(const ot::Plot1DCfg& _plotCfg);
};
