#pragma once
#include "OTServiceFoundation/MenuButtonDescription.h"
#include "OTServiceFoundation/UiComponent.h"
#include "ActionAndFunctionHandler.h"
#include "SelectionChangedObserver.h"

class PlotHandler :public ActionAndFunctionHandler, public SelectionChangedObserver
{
public:
	void addButtons(ot::components::UiComponent* _uiComponent, const std::string& _pageName);

	std::string createPlotAction(ot::JsonDocument& _document);

protected:
	virtual bool handleAction(const std::string& _action, ot::JsonDocument& _doc) override;

private:
	ot::MenuButtonDescription m_btnCreatePlot;
	
	// Inherited via SelectionChangedObserver
	void updatedSelection(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons) override;

};
