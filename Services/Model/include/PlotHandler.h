#pragma once

#include "OTServiceFoundation/UiComponent.h"
#include "SelectionChangedObserver.h"
#include "EntityMetadataSeries.h"
#include "EntityResult1DPlot.h"
#include "OTGui/Plot1DCurveCfg.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "NewModelStateInfo.h"

class PlotHandler : public SelectionChangedObserver
{
public:
	PlotHandler();

	void addButtons(ot::components::UiComponent* _uiComponent);

private:
	ot::ToolBarButtonCfg m_btnCreatePlot;
	ot::ToolBarButtonCfg m_btnAddCurveToPlot;
	
	const std::string c_groupName = "Plots";

	ot::ButtonHandler m_buttonHandler;
	void handleCreatePlot();
	void handleAddCurveToPlot();

	void updatedSelection(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons) override;
	std::list<EntityMetadataSeries*> getSelectedSeriesMetadata();
	std::list<EntityResult1DPlot*> getSelectedPlots();
	std::string getFreePlotName();

	void createCurves(std::list<EntityMetadataSeries*>& _seriesMetadata, ot::NewModelStateInfo& _modelStateInformation, const std::string& _nameBase);
};
