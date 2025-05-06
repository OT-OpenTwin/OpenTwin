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
	//! @brief Creating a series metadata entity and curve entity. The curve entity name needs to be a full name, basically <plot_entity_name>/curve_name
	//! The datapoints are directly stored in the result database, curve entity is directly stored as well. 
	//! Modelstate is created when the final plot is build
	//! @param _dataSetDescription 
	//! @param _config 
	//! @param _seriesName Name of the created series metadata entity. The the dataset folder name is automatically added in front of the series name.
	void addCurve(DatasetDescription&& _dataSetDescription, ot::Plot1DCurveCfg& _config, const std::string& _seriesName);
	
	//! @brief Creating one series metadata entity for all dataset descriptions and one curve entity. The curve entity name needs to be a full name, basically <plot_entity_name>/curve_name
	//! The datapoints are directly stored in the result database, curve entity is directly stored as well. 
	//! Modelstate is created when the final plot is build
	//! @param _dataSetDescriptions 
	//! @param _config 
	//! @param _seriesName Name of the created series metadata entity. The the dataset folder name is automatically added in front of the series name.
	void addCurve(std::list<DatasetDescription>&& _dataSetDescriptions, ot::Plot1DCurveCfg& _config, const std::string& _seriesName);
	
	//! @brief Creates a plot entity
	//! @param _plotCfg 
	//! @param _saveModelState 
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
