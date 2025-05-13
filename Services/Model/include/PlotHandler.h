#pragma once
#include "OTServiceFoundation/MenuButtonDescription.h"
#include "OTServiceFoundation/UiComponent.h"
#include "ActionAndFunctionHandler.h"
#include "SelectionChangedObserver.h"
#include "EntityMetadataSeries.h"
#include "EntityResult1DPlot.h"
#include "OTGui/Plot1DCurveCfg.h"
#include "OTModelAPI/NewModelStateInformation.h"

class PlotHandler :public ActionAndFunctionHandler, public SelectionChangedObserver
{
public:
	void addButtons(ot::components::UiComponent* _uiComponent, const std::string& _pageName);

	std::string createPlotAction(ot::JsonDocument& _document);
	std::string addCurvesToPlot(ot::JsonDocument& _document);

protected:
	virtual bool handleAction(const std::string& _action, ot::JsonDocument& _doc) override;
private:
	ot::MenuButtonDescription m_btnCreatePlot;
	ot::MenuButtonDescription m_btnAddCurveToPlot;
	
	void updatedSelection(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons) override;
	std::list<EntityMetadataSeries*> getSelectedSeriesMetadata();
	std::list<EntityResult1DPlot*> getSelectedPlots();
	std::string getFreePlotName();

	std::list<ot::Plot1DCurveCfg> createCurves(std::list<EntityMetadataSeries*>& _seriesMetadata, ot::NewModelStateInformation& _modelStateInformation, const std::string& _nameBase);
};
