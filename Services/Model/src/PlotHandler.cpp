#include "stdafx.h"
#include "PlotHandler.h"
#include "Model.h"
#include "EntityResult1DPlot.h"
#include "EntityResult1DCurve.h"
#include "EntityMetadataSeries.h"
#include "Application.h"
#include "OTCore/Logger.h"
#include "MetadataEntityInterface.h"
#include "CurveFactory.h"
#include "OTModelAPI/NewModelStateInformation.h"
#include "OTGui/PainterRainbowIterator.h"
#include "QueuingHttpRequestsRAII.h"

void PlotHandler::addButtons(ot::components::UiComponent* _uiComponent, const std::string& _pageName)
{
	const std::string groupName = "Plots";
	_uiComponent->addMenuGroup(_pageName, groupName);
	
	m_btnCreatePlot.SetDescription(_pageName, groupName, "CreatePlot", "Create Plot");
	_uiComponent->addMenuButton(m_btnCreatePlot, ot::LockModelWrite, "Plot1DVisible");
	
	m_btnAddCurveToPlot.SetDescription(_pageName, groupName, "AddCurveToPlot", "Add Curve to Plot");
	_uiComponent->addMenuButton(m_btnAddCurveToPlot, ot::LockModelWrite, "Plot1DVisible");

	//m_connectionManager.bindHandler(this, &PlotHandler::createPlotAction, m_btnCreatePlot.GetFullDescription(), ot::SECURE_MESSAGE_TYPES); //crashes because of action handler
}

std::string PlotHandler::createPlotAction(ot::JsonDocument& _document)
{
	QueuingHttpRequestsRAII uiQueue;

	auto selectedSeriesMetadata = getSelectedSeriesMetadata();
	if (selectedSeriesMetadata.size() == 0)
	{
		return "";
	}

	//Find a free name for a plot
	const std::string plotName = getFreePlotName();
	if (plotName.empty())
	{
		return "";
	}

	ot::NewModelStateInformation newModelStateInformation;
	createCurves(selectedSeriesMetadata, newModelStateInformation, plotName);


	//Finally we create the plot entity
	Model* model = Application::instance()->getModel();
	EntityResult1DPlot newPlot(model->createEntityUID(), nullptr, nullptr, nullptr, &model->getClassFactory(), Application::instance()->getServiceName());
	newPlot.setName(plotName);

	ot::Plot1DCfg plotCfg;
	const std::string shortName = plotName.substr(plotName.find_last_of("/") + 1);
	plotCfg.setTitle(shortName);
	newPlot.createProperties();
	newPlot.setPlot(plotCfg);
	newPlot.StoreToDataBase();
	newModelStateInformation.m_topologyEntityIDs.push_back(newPlot.getEntityID());
	newModelStateInformation.m_topologyEntityVersions.push_back(newPlot.getEntityStorageVersion());
	newModelStateInformation.m_forceVisible.push_back(false);

	//Store state
	std::list<ot::UID> noDataEntities{};
	model->addEntitiesToModel(
		newModelStateInformation.m_topologyEntityIDs, 
		newModelStateInformation.m_topologyEntityVersions, 
		newModelStateInformation.m_forceVisible, 
		noDataEntities, noDataEntities, noDataEntities, "Created new plot for existing series metadata", true, true);

	return std::string();
}

std::string PlotHandler::addCurvesToPlot(ot::JsonDocument& _document)
{
	QueuingHttpRequestsRAII uiQueue;
	
	auto selectedSeriesMetadata = getSelectedSeriesMetadata();
	auto selectedPlots = getSelectedPlots();
	if (selectedSeriesMetadata.size() == 0 || selectedPlots.size() == 0)
	{
		return "";
	}
	
	ot::NewModelStateInformation newModelStateInformation, plotsForUpdate;
	bool storeSecond = false;
	for (EntityResult1DPlot* selectedPlot : selectedPlots)
	{
		const std::string plotName = selectedPlot->getName();
		createCurves(selectedSeriesMetadata, newModelStateInformation, plotName);		
	}

	Model* model = Application::instance()->getModel();
	const std::string comment = "Added curves to plots";
	 
	std::list<ot::UID> noDataEntities{};
	model->addEntitiesToModel(
		newModelStateInformation.m_topologyEntityIDs,
		newModelStateInformation.m_topologyEntityVersions,
		newModelStateInformation.m_forceVisible,
		noDataEntities, noDataEntities, noDataEntities, comment, !storeSecond, true);
	
	if (storeSecond)
	{
		model->updateTopologyEntities(plotsForUpdate.m_topologyEntityIDs, plotsForUpdate.m_topologyEntityVersions, comment);
	}
	
	const bool overrideContent = true;
	for (EntityResult1DPlot* selectedPlot : selectedPlots)
	{	
		Application::instance()->getVisualisationHandler().handleVisualisationRequest(selectedPlot->getEntityID(), OT_ACTION_CMD_VIEW1D_Setup, overrideContent);
	}
	return std::string();
}

bool PlotHandler::handleAction(const std::string& _action, ot::JsonDocument& _doc)
{
	bool actionIsHandled = false;

	if (_action == m_btnCreatePlot.GetFullDescription())
	{
		createPlotAction(_doc);
		actionIsHandled = true;
	}
	else if (_action == m_btnAddCurveToPlot.GetFullDescription())
	{
		addCurvesToPlot(_doc);
		actionIsHandled = true;
	}
	return actionIsHandled;
}

std::list<EntityMetadataSeries*> PlotHandler::getSelectedSeriesMetadata()
{
	SelectionHandler& selectionHandler = Application::instance()->getSelectionHandler();
	std::list<ot::UID> selectedEntityIDs = selectionHandler.getSelectedEntityIDs();
	
	Model* model = Application::instance()->getModel();

	std::list<EntityMetadataSeries*> selectedSeriesMetadata;
	for (ot::UID entityID : selectedEntityIDs)
	{
		EntityBase* entityBase = model->getEntityByID(entityID);
		if (entityBase != nullptr)
		{
			EntityMetadataSeries* seriesMetadata = dynamic_cast<EntityMetadataSeries*>(entityBase);
			if (seriesMetadata != nullptr)
			{
				selectedSeriesMetadata.push_back(seriesMetadata);
			}
		}
		else
		{
			OT_LOG_E("For developer: Here is an entity listed as selected, but not part of the model");
		}
	}
	return selectedSeriesMetadata;
}

std::list<EntityResult1DPlot*> PlotHandler::getSelectedPlots()
{
	SelectionHandler& selectionHandler = Application::instance()->getSelectionHandler();
	std::list<ot::UID> selectedEntityIDs = selectionHandler.getSelectedEntityIDs();

	Model* model = Application::instance()->getModel();

	std::list<EntityResult1DPlot*> selectedPlots;
	for (ot::UID entityID : selectedEntityIDs)
	{
		EntityBase* entityBase = model->getEntityByID(entityID);
		if (entityBase != nullptr)
		{
			EntityResult1DPlot* selectedPlot = dynamic_cast<EntityResult1DPlot*>(entityBase);
			if (selectedPlot != nullptr)
			{
				selectedPlots.push_back(selectedPlot);
			}
		}
		else
		{
			OT_LOG_E("For developer: Here is an entity listed as selected, but not part of the model");
		}
	}
	return selectedPlots;
}

std::string PlotHandler::getFreePlotName()
{
	Model* model = Application::instance()->getModel();
	const std::string folderName = "Plots/";
	std::string shortName = "Plot";
	std::string plotName = folderName + shortName;
	auto entityNameToID = model->getEntityNameToIDMap();
	uint32_t counter = 0;
	uint32_t maxCounter = 10000;

	while (entityNameToID.find(plotName) != entityNameToID.end() && counter < maxCounter)
	{
		shortName = "Plot_" + std::to_string(counter);
		plotName = folderName + shortName;
		counter++;
	}

	if (counter == maxCounter - 1)
	{
		OT_LOG_E("Only " + std::to_string(maxCounter) + " plots are supported.");
		return "";
	}
	else
	{
		return plotName;
	}
}

void PlotHandler::createCurves(std::list<EntityMetadataSeries*>& _seriesMetadata, ot::NewModelStateInformation& _modelStateInformation, const std::string& _nameBase)
{
	ot::PainterRainbowIterator colourIt;
	MetadataEntityInterface metadataEntityInteraface;
	Model* model = Application::instance()->getModel();

	for (EntityMetadataSeries* seriesMetadata : _seriesMetadata)
	{
		MetadataSeries series = metadataEntityInteraface.createSeries(seriesMetadata);
		ot::Plot1DCurveCfg curveConfig;
		const std::string fullName = seriesMetadata->getName();
		const std::string shortName = fullName.substr(fullName.find_last_of("/") + 1);
		auto painter = colourIt.getNextPainter();
		curveConfig.setLinePenPainter(painter.release());

		CurveFactory::addToConfig(series, curveConfig);

		EntityResult1DCurve newCurve(model->createEntityUID(), nullptr, nullptr, nullptr, &model->getClassFactory(), Application::instance()->getServiceName());
		newCurve.setName(_nameBase + "/" + shortName);
		newCurve.createProperties();
		newCurve.setCurve(curveConfig);
		newCurve.StoreToDataBase();

		_modelStateInformation.m_topologyEntityIDs.push_back(newCurve.getEntityID());
		_modelStateInformation.m_topologyEntityVersions.push_back(newCurve.getEntityStorageVersion());
		_modelStateInformation.m_forceVisible.push_back(false);
	}
}

void PlotHandler::updatedSelection(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons)
{
	bool seriesSelected = false;
	bool plotSelected = false;
	for (EntityBase* selectedEntity : _selectedEntities)
	{
		EntityMetadataSeries* series = dynamic_cast<EntityMetadataSeries*>(selectedEntity);
		if (series != nullptr)
		{
			seriesSelected = true;
		}
		else
		{
			EntityResult1DPlot* plot = dynamic_cast<EntityResult1DPlot*>(selectedEntity);
			if (plot != nullptr)
			{
				plotSelected = true;
			}
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

	if (seriesSelected && plotSelected)
	{
		_enabledButtons.push_back(m_btnAddCurveToPlot.GetFullDescription());
	}
	else
	{
		_disabledButtons.push_back(m_btnAddCurveToPlot.GetFullDescription());
	}
}


