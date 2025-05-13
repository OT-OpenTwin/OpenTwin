#include "stdafx.h"
#include "PlotHandler.h"
#include "Model.h"
#include "EntityResult1DPlot_New.h"
#include "EntityResult1DCurve_New.h"
#include "EntityMetadataSeries.h"
#include "Application.h"
#include "OTCore/Logger.h"
#include "MetadataEntityInterface.h"
#include "CurveFactory.h"
#include "OTModelAPI/NewModelStateInformation.h"
#include "OTGui/PainterRainbowIterator.h"

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
	SelectionHandler& selectionHandler = Application::instance()->getSelectionHandler();
	Model* model = Application::instance()->getModel();

	std::list<ot::UID> selectedEntityIDs =	selectionHandler.getSelectedEntityIDs();
	
	std::list<EntityMetadataSeries*> selectedSeriesMetadata;
	for (ot::UID entityID : selectedEntityIDs)
	{
		EntityBase* entityBase =  model->getEntityByID(entityID);
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

	if (selectedSeriesMetadata.size() == 0)
	{
		return "";
	}

	const std::string folderName = "Plots/";
	std::string shortName = "Plot";
	std::string plotName = folderName + shortName;
	auto entityNameToID =	model->getEntityNameToIDMap();
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

	EntityResult1DPlot_New newPlot(model->createEntityUID(), nullptr, nullptr, nullptr, &model->getClassFactory(), Application::instance()->getServiceName());
	newPlot.setName(plotName);

	ot::Plot1DCfg plotCfg;
	plotCfg.setTitle(shortName);

	ot::NewModelStateInformation newModelStateInformation;
	std::list<std::string> parameterLabels;
	std::list<std::string> quantityLabel;
	std::list<ot::Plot1DCurveCfg> curves;
	ot::PainterRainbowIterator colourIt;
	for (EntityMetadataSeries* seriesMetadata : selectedSeriesMetadata)
	{
		MetadataEntityInterface metadataEntityInteraface;
		MetadataSeries series =	metadataEntityInteraface.createSeries(seriesMetadata);
		ot::Plot1DCurveCfg curveConfig;
		const std::string fullName = seriesMetadata->getName();
		const std::string shortName = fullName.substr(fullName.find_last_of("/") + 1);
		curveConfig.setTitle(shortName);
		curveConfig.setXAxisTitle("X-Title");
		curveConfig.setYAxisTitle("Y-Title");
		curveConfig.setXAxisUnit("X-Unit");
		curveConfig.setYAxisUnit("Y-Unit");
		auto painter =	colourIt.getNextPainter();
		curveConfig.setLinePen(painter.release());

		CurveFactory::addToConfig(series, curveConfig);
		
		for (auto& parameterDescription : curveConfig.getQueryInformation().m_parameterDescriptions)
		{
			parameterLabels.push_back(parameterDescription.m_label);
		}
		quantityLabel.push_back(curveConfig.getQueryInformation().m_quantityDescription.m_label);

		EntityResult1DCurve_New newCurve(model->createEntityUID(), nullptr, nullptr, nullptr, &model->getClassFactory(), Application::instance()->getServiceName());
		newCurve.setName(plotName + "/" + shortName);
		newCurve.createProperties();
		newCurve.setCurve(curveConfig);
		newCurve.StoreToDataBase();

		newModelStateInformation.m_topologyEntityIDs.push_back(newCurve.getEntityID());
		newModelStateInformation.m_topologyEntityVersions.push_back(newCurve.getEntityStorageVersion());
		newModelStateInformation.m_forceVisible.push_back(false);
	}

	parameterLabels.unique();
	quantityLabel.unique();

	newPlot.setFamilyOfCurveProperties(parameterLabels, quantityLabel);
	newPlot.createProperties();
	newPlot.setPlot(plotCfg);
	newPlot.StoreToDataBase();
	newModelStateInformation.m_topologyEntityIDs.push_back(newPlot.getEntityID());
	newModelStateInformation.m_topologyEntityVersions.push_back(newPlot.getEntityStorageVersion());
	newModelStateInformation.m_forceVisible.push_back(false);

	std::list<ot::UID> noDataEntities{};
	model->addEntitiesToModel(
		newModelStateInformation.m_topologyEntityIDs, 
		newModelStateInformation.m_topologyEntityVersions, 
		newModelStateInformation.m_forceVisible, 
		noDataEntities, noDataEntities, noDataEntities, "Created new plot for existing series metadata", true, true);

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
	return actionIsHandled;
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


