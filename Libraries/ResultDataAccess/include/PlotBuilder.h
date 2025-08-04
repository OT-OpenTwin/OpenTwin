#pragma once
#include <string>

#include "DatasetDescription.h"
#include "OTGui/Plot1DCurveCfg.h"
#include "OTGui/Plot1DCfg.h"
#include "OTModelAPI/NewModelStateInformation.h"
#include "ResultCollectionExtender.h"
#include "EntityResult1DCurve.h"

class __declspec(dllexport) PlotBuilder
{
public:
	PlotBuilder(ResultCollectionExtender& _extender);
	//! @brief Creating a series metadata entity.
	//! The datapoints are directly stored in the result database. 
	//! Modelstate is created when the final plot is build
	//! @param _dataSetDescription 
	//! @param _config Curve title is used for entity name and added at the back of the plot entity name
	//! @param _seriesName Name of the created series metadata entity. The the dataset folder name is automatically added in front of the series name.
	void addCurve(DatasetDescription&& _dataSetDescription, ot::Plot1DCurveCfg& _config, const std::string& _seriesName);
	
	//! @brief Creating one series metadata entity for all dataset descriptions. 
	//! The datapoints are directly stored in the result database, curve entity is directly stored as well. 
	//! Modelstate is created when the final plot is build
	//! @param _dataSetDescriptions 
	//! @param _config Curve title is used for entity name and added at the back of the plot entity name
	//! @param _seriesName Name of the created series metadata entity. The the dataset folder name is automatically added in front of the series name.
	void addCurve(std::list<DatasetDescription>&& _dataSetDescriptions, ot::Plot1DCurveCfg& _config, const std::string& _seriesName);
	
	//! @brief Final method. All added curves and the plot are stored and added to the model state.
	//! @param _plotCfg Needs entity name to be set.
	//! @param _saveModelState 
	void buildPlot(ot::Plot1DCfg& _plotCfg, bool _saveModelState = true);

	uint64_t getNumberOfCurves() { return m_curves.size(); }

private:
	ot::NewModelStateInformation m_newModelStateInformation;
	ResultCollectionExtender& m_extender;
	std::list<std::string> m_parameterLabels;
	std::list<std::string> m_quantityLabel;
	std::list<EntityResult1DCurve> m_curves;

	bool validityCheck(std::list<DatasetDescription>& _dataSetDescriptions, ot::Plot1DCurveCfg& _config);

	void storeCurve(std::list<DatasetDescription>&& _dataSetDescriptions, ot::Plot1DCurveCfg& _config, const std::string& _seriesName);
	
	void clearBuffer();

	void createPlot(ot::Plot1DCfg& _plotCfg);
};
