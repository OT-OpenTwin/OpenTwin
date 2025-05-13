#include "stdafx.h"
#include "PlotHandler.h"
#include "Model.h"
#include "EntityResult1DPlot_New.h"
#include "EntityResult1DCurve_New.h"
#include "EntityMetadataSeries.h"

void PlotHandler::addButtons(ot::components::UiComponent* _uiComponent, const std::string& _pageName)
{
	const std::string groupName = "Plots";
	_uiComponent->addMenuGroup(_pageName, groupName);
	
	m_btnCreatePlot.SetDescription(_pageName, groupName, "CreatePlot", "Create Plot");
	_uiComponent->addMenuButton(m_btnCreatePlot, ot::LockModelWrite, "Plot1DVisible");
	
	//m_connectionManager.bindHandler(this, &PlotHandler::createPlotAction, m_btnCreatePlot.GetFullDescription(), ot::SECURE_MESSAGE_TYPES); //crashes because of action handler
}

std::string PlotHandler::createPlotAction(ot::JsonDocument& _document)
{
	return std::string();
}

bool PlotHandler::handleAction(const std::string& _action, ot::JsonDocument& _doc)
{
	return false;
}

void PlotHandler::updatedSelection(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons)
{
	bool seriesSelected = false;
	for (EntityBase* selectedEntity : _selectedEntities)
	{
		EntityMetadataSeries* series = dynamic_cast<EntityMetadataSeries*>(selectedEntity);
		if (series != nullptr)
		{
			seriesSelected = true;
			break;
		}
	}

	if (seriesSelected)
	{
		_enabledButtons.push_back(m_btnCreatePlot.GetFullDescription());
	}
	else
	{
		_disabledButtons.push_back(m_btnCreatePlot.GetFullDescription());
	}
}


