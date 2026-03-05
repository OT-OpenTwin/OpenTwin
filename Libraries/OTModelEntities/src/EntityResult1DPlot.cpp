// @otlicense
// File: EntityResult1DPlot.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/EntityName.h"
#include "OTGui/VisualisationTypes.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/PropertyHelper.h"
#include "OTModelEntities/EntityResult1DPlot.h"
#include "OTModelEntities/EntityResult1DCurve.h"

// std header
#include <algorithm>

static EntityFactoryRegistrar<EntityResult1DPlot> registrar("EntityResult1DPlot");

EntityResult1DPlot::EntityResult1DPlot(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	:EntityContainer(_ID,_parent,_obs,_ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/Plot1DVisible");
	treeItem.setHiddenIcon("Default/Plot1DHidden");
	this->setDefaultTreeItem(treeItem);

	ot::VisualisationTypes visTypes = getVisualizationTypes();
	visTypes.addPlot1DVisualisation();
	this->setDefaultVisualizationTypes(visTypes);
}

void EntityResult1DPlot::storeToDataBase()
{
	EntityContainer::storeToDataBase();
}

void EntityResult1DPlot::addVisualizationNodes()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);

	for (auto child : getChildrenList())
	{
		child->addVisualizationNodes();
	}
}

bool EntityResult1DPlot::updateFromProperties()
{
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	//Properties that require the curve data to be fetched again
	bool requiresDataToBeFetched = false;
	auto numberOfCurves = PropertyHelper::getBoolProperty(this, "Number of curves", "Curve limit");
	requiresDataToBeFetched |=	numberOfCurves->needsUpdate();
	auto numberOfCurvesMax = PropertyHelper::getIntegerProperty(this, "Max", "Curve limit");
	requiresDataToBeFetched |= numberOfCurvesMax->needsUpdate();
	requiresDataToBeFetched |= m_querySettings.requiresUpdate(this);
	
	auto showFullMatrixProp =	PropertyHelper::getBoolProperty(this,"Show full matrix");
	requiresDataToBeFetched |= showFullMatrixProp->needsUpdate();

	if (!showFullMatrixProp->getValue())
	{
		auto showMatrixRowProp = PropertyHelper::getIntegerProperty(this, "Show matrix row entry");
		requiresDataToBeFetched |= showMatrixRowProp->needsUpdate();

		auto showMatrixColumnProp = PropertyHelper::getIntegerProperty(this, "Show matrix column entry");
		requiresDataToBeFetched |= showMatrixColumnProp->needsUpdate();
	}

	requiresDataToBeFetched |= PropertyHelper::getSelectionProperty(this, "X axis parameter", "Curve set")->needsUpdate();


	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setVisualisationType(OT_ACTION_CMD_VIEW1D_Setup);
	visualisationCfg.setOverrideViewerContent(requiresDataToBeFetched);
	visualisationCfg.setAsActiveView(true);
	getObserver()->requestVisualisation(getEntityID(), visualisationCfg);

	getProperties().forceResetUpdateForAllProperties();

	return updatePropertyVisibilities();
}

bool EntityResult1DPlot::updatePropertyVisibilities()
{
	bool updatePropertiesGrid = false;
	
	EntityPropertiesBoolean* gridVisibility = PropertyHelper::getBoolProperty(this, "Grid"); 
	EntityPropertiesGuiPainter* gridColor = PropertyHelper::getPainterProperty(this,"Grid color");

	if (gridVisibility->getValue() != gridColor->getVisible())
	{
		gridColor->setVisible(gridVisibility->getValue());
		gridColor->resetNeedsUpdate();
		updatePropertiesGrid = true;
	}

	EntityPropertiesBoolean* autoscaleX = PropertyHelper::getBoolProperty(this, "Autoscale", "X axis");
	EntityPropertiesDouble* minX = PropertyHelper::getDoubleProperty(this, "Min", "X axis");
	EntityPropertiesDouble* maxX = PropertyHelper::getDoubleProperty(this, "Max", "X axis");

	if (autoscaleX->getValue() == minX->getVisible())
	{
		minX->setVisible(!autoscaleX->getValue());
		maxX->setVisible(!autoscaleX->getValue());
		minX->resetNeedsUpdate();
		maxX->resetNeedsUpdate();
		updatePropertiesGrid = true;
	}

	EntityPropertiesBoolean* autoscaleY = PropertyHelper::getBoolProperty(this,"Autoscale", "Y axis");
	EntityPropertiesDouble* minY = PropertyHelper::getDoubleProperty(this, "Min","Y axis");
	EntityPropertiesDouble* maxY = PropertyHelper::getDoubleProperty(this, "Max","Y axis");

	if (autoscaleY->getValue() == minY->getVisible())
	{
		minY->setVisible(!autoscaleY->getValue());
		maxY->setVisible(!autoscaleY->getValue());
		minY->resetNeedsUpdate();
		maxY->resetNeedsUpdate();
		updatePropertiesGrid = true;
	}

	updatePropertiesGrid |= m_querySettings.updatePropertyVisibility(this);

	return updatePropertiesGrid;
}

void EntityResult1DPlot::hideAxisQuantityProperties()
{
	PropertyHelper::getSelectionProperty(this, "Quantity", "X axis")->setVisible(false);
	PropertyHelper::getSelectionProperty(this, "Quantity", "Y axis")->setVisible(false);
}

void EntityResult1DPlot::addChild(EntityBase* _child)
{
	EntityContainer::addChild(_child);
	setQuerySelections();
}

void EntityResult1DPlot::removeChild(EntityBase* _child)
{
	EntityContainer::removeChild(_child);
	setQuerySelections();
}

void EntityResult1DPlot::createProperties()
{
	//Query options are set in the addChild and removeChild methods
	EntityPropertiesSelection::createProperty("Curve set", "X axis parameter", {}, "", "default", getProperties());
	std::list<std::string> allQueryOptions{ "" };
	m_querySettings.setQueryDefinitions(allQueryOptions);
	m_querySettings.setProperties(this);

	const std::list<std::string> plotTypeOptions = ot::Plot1DCfg::getPlotTypeStringList();
	const std::list<std::string> quantityOptions = ot::Plot1DAxisCfg::getAxisQuantityStringList();

	EntityPropertiesSelection::createProperty("General", "Plot type", plotTypeOptions, ot::Plot1DCfg::toString(ot::Plot1DCfg::Cartesian), "", getProperties());
	EntityPropertiesBoolean::createProperty("General", "Grid", true, "", getProperties());
	EntityPropertiesGuiPainter::createProperty("General", "Grid color", new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::PlotGrid), "", getProperties());
	EntityPropertiesBoolean::createProperty("General", "Legend", true, "", getProperties());

	EntityPropertiesSelection::createProperty("X axis", "Quantity", quantityOptions, ot::Plot1DAxisCfg::toString(ot::Plot1DAxisCfg::XData), "", getProperties());
	EntityPropertiesBoolean::createProperty("X axis", "Logscale", false, "", getProperties());
	EntityPropertiesBoolean::createProperty("X axis", "Autoscale", true, "", getProperties());
	EntityPropertiesDouble::createProperty("X axis", "Min", 0.0, "", getProperties());
	EntityPropertiesDouble::createProperty("X axis", "Max", 0.0, "", getProperties());
	EntityPropertiesBoolean::createProperty("X axis", "Automatic label", true, "", getProperties());
	EntityPropertiesString::createProperty("X axis", "Label override", "", "", getProperties());

	EntityPropertiesSelection::createProperty("Y axis", "Quantity", quantityOptions, ot::Plot1DAxisCfg::toString(ot::Plot1DAxisCfg::Real), "", getProperties());
	EntityPropertiesBoolean::createProperty("Y axis", "Logscale", false, "", getProperties());
	EntityPropertiesBoolean::createProperty("Y axis", "Autoscale", true, "", getProperties());
	EntityPropertiesDouble::createProperty("Y axis", "Min", 0.0, "", getProperties());
	EntityPropertiesDouble::createProperty("Y axis", "Max", 0.0, "", getProperties());
	EntityPropertiesBoolean::createProperty("Y axis", "Automatic label", true, "", getProperties());
	EntityPropertiesString::createProperty("Y axis", "Label override", "", "", getProperties());
	EntityPropertiesBoolean::createProperty("Y axis", "Show full matrix", true, "", getProperties());
	EntityPropertiesInteger::createProperty("Y axis", "Show matrix row entry", 1,1,60, "", getProperties());
	EntityPropertiesInteger::createProperty("Y axis", "Show matrix column entry", 1,1,60, "", getProperties());

	EntityPropertiesBoolean::createProperty("Curve limit", "Number of curves", true, "default", getProperties());
	EntityPropertiesInteger::createProperty("Curve limit", "Max", 25, "default", getProperties());

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}


const ot::Plot1DCfg EntityResult1DPlot::getPlot() {

	const ot::Painter2D* gridColour = PropertyHelper::getPainterPropertyValue(this, "Grid color");

	const std::string entityName = getName();
	auto shortName = ot::EntityName::getSubName(entityName);
	std::string title("");
	if (shortName.has_value()) {
		title = shortName.value();
	}
	else {
		assert(false);
	}

	const std::string plotType = PropertyHelper::getSelectionPropertyValue(this, "Plot type");

	const bool gridVisible = PropertyHelper::getBoolPropertyValue(this, "Grid");
	const bool legendVisible = PropertyHelper::getBoolPropertyValue(this, "Legend");

	const std::string xAxisQuantity = PropertyHelper::getSelectionPropertyValue(this, "Quantity", "X axis");
	const bool logScaleX = PropertyHelper::getBoolPropertyValue(this, "Logscale", "X axis");
	const bool autoScaleX = PropertyHelper::getBoolPropertyValue(this, "Autoscale", "X axis");
	const double minX = PropertyHelper::getDoublePropertyValue(this, "Min", "X axis");
	const double maxX = PropertyHelper::getDoublePropertyValue(this, "Max", "X axis");

	const std::string yAxisQuantity = PropertyHelper::getSelectionPropertyValue(this, "Quantity", "Y axis");
	const bool logScaleY = PropertyHelper::getBoolPropertyValue(this, "Logscale", "Y axis");
	const bool autoScaleY = PropertyHelper::getBoolPropertyValue(this, "Autoscale", "Y axis");
	const double minY = PropertyHelper::getDoublePropertyValue(this, "Min", "Y axis");
	const double maxY = PropertyHelper::getDoublePropertyValue(this, "Max", "Y axis");

	const int32_t maxNbOfCurves = PropertyHelper::getIntegerPropertyValue(this, "Max", "Curve limit");
	const bool useCurveLimit = PropertyHelper::getBoolPropertyValue(this, "Number of curves", "Curve limit");

	const std::string xAxisParameter = PropertyHelper::getSelectionPropertyValue(this, "X axis parameter","Curve set");

	const bool automaticLabelX = PropertyHelper::getBoolPropertyValue(this, "Automatic label", "X axis");
	const bool automaticLabelY = PropertyHelper::getBoolPropertyValue(this, "Automatic label", "Y axis");
	const std::string labelY = PropertyHelper::getStringPropertyValue(this, "Label override", "Y axis");
	const std::string labelX = PropertyHelper::getStringPropertyValue(this, "Label override", "X axis");

	const bool showEntireMatrix = PropertyHelper::getBoolPropertyValue(this, "Show full matrix");
	const int32_t showMatrixRowValue = PropertyHelper::getIntegerPropertyValue(this, "Show matrix row entry");
	const int32_t showMatrixColumnValue = PropertyHelper::getIntegerPropertyValue(this, "Show matrix column entry");

	std::list<ot::ValueComparisonDescription> queries = m_querySettings.getValueComparisonDefinitions(this);

	ot::Plot1DCfg config;
	config.setEntityName(getName());
	config.setEntityID(getEntityID());
	config.setEntityVersion(getEntityStorageVersion());

	config.setTitle(title);
	config.setCollectionName(DataBase::instance().getCollectionName());
	config.setOldTreeIcons(ot::NavigationTreeItemIcon("Plot1DVisible", "Plot1DHidden"));

	config.setXAxisParameter(xAxisParameter);

	config.setPlotType(ot::Plot1DCfg::stringToPlotType(plotType));

	config.setGridColor(gridColour->createCopy());
	config.setGridVisible(gridVisible);
	config.setLegendVisible(legendVisible);

	config.setXAxisIsLogScale(logScaleX);
	config.setXAxisIsAutoScale(autoScaleX);
	config.setXAxisMin(minX);
	config.setXAxisMax(maxX);
	config.setXAxisQuantity(ot::Plot1DAxisCfg::stringToAxisQuantity(xAxisQuantity));
	
	config.setYAxisIsLogScale(logScaleY);
	config.setYAxisIsAutoScale(autoScaleY);
	config.setYAxisMin(minY);
	config.setYAxisMax(maxY);
	config.setYAxisQuantity(ot::Plot1DAxisCfg::stringToAxisQuantity(yAxisQuantity));

	config.setShowEntireMatrix(showEntireMatrix);
	config.setShowMatrixColumnEntry(showMatrixColumnValue);
	config.setShowMatrixRowEntry(showMatrixRowValue);

	config.setYLabelAxisAutoDetermine(automaticLabelY);
	config.setXLabelAxisAutoDetermine(automaticLabelX);
	config.setAxisLabelY(labelY);
	config.setAxisLabelX(labelX);


	config.setQueries(queries);

	config.setLimitOfCurves(maxNbOfCurves);
	config.setUseLimitNbOfCurves(useCurveLimit);

	return config;
}

bool EntityResult1DPlot::visualisePlot()
{
	return true;
}

void EntityResult1DPlot::setQuerySelections()
{
	std::list<std::string> filterOptions, parameterOptions;
	for (EntityBase* child : getChildrenList())
	{
		auto curve = dynamic_cast<EntityResult1DCurve*>(child);
		if (curve != nullptr)
		{
			const ot::QueryInformation& queryInformation = curve->getQueryInformation();
			filterOptions.push_back(queryInformation.m_quantityDescription.m_label);
			for (auto& parameterDescr : queryInformation.m_parameterDescriptions)
			{
				if (find(filterOptions.begin(), filterOptions.end(), parameterDescr.m_label) == filterOptions.end())
				{
					filterOptions.push_back(parameterDescr.m_label);
				}
				if (find(parameterOptions.begin(), parameterOptions.end(), parameterDescr.m_label) == parameterOptions.end())
				{
					parameterOptions.push_back(parameterDescr.m_label);
				}
			}
		}
	}
	filterOptions.push_back("");

	PropertyHelper::getSelectionProperty(this, "X axis parameter", "Curve set")->resetOptions(parameterOptions);
	PropertyHelper::getSelectionProperty(this, "X axis parameter", "Curve set")->setValue(*parameterOptions.begin());
	m_querySettings.updateQuerySettings(this, filterOptions);
}

void EntityResult1DPlot::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityContainer::addStorageData(storage);
}

void EntityResult1DPlot::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);
	m_querySettings.reload(this);
	resetModified();
}

void EntityResult1DPlot::setPlot(const ot::Plot1DCfg& _config)
{
	PropertyHelper::setPainterPropertyValue(_config.getGridColor(), this, "Grid color");
	PropertyHelper::setSelectionPropertyValue(ot::Plot1DCfg::toString(_config.getPlotType()), this, "Plot type");
	
	PropertyHelper::setBoolPropertyValue(_config.getGridVisible(), this, "Grid");
	PropertyHelper::setBoolPropertyValue(_config.getLegendVisible(), this, "Legend");

	PropertyHelper::setSelectionPropertyValue(ot::Plot1DAxisCfg::toString(_config.getXAxisQuantity()), this, "Quantity", "X axis");
	PropertyHelper::setBoolPropertyValue(_config.getXAxisIsLogScale(), this, "Logscale", "X axis");
	PropertyHelper::setBoolPropertyValue(_config.getXAxisIsAutoScale(), this, "Autoscale", "X axis");
	PropertyHelper::setDoublePropertyValue(_config.getXAxisMin(), this, "Min", "X axis");
	PropertyHelper::setDoublePropertyValue(_config.getXAxisMax(), this, "Max", "X axis");

	PropertyHelper::setSelectionPropertyValue(ot::Plot1DAxisCfg::toString(_config.getYAxisQuantity()), this, "Quantity", "Y axis");
	PropertyHelper::setBoolPropertyValue(_config.getYAxisIsLogScale(), this, "Logscale", "Y axis");
	PropertyHelper::setBoolPropertyValue(_config.getYAxisIsAutoScale(), this, "Autoscale", "Y axis");
	PropertyHelper::setDoublePropertyValue(_config.getYAxisMin(), this, "Min", "Y axis");
	PropertyHelper::setDoublePropertyValue(_config.getYAxisMax(), this, "Max", "Y axis");

	PropertyHelper::setStringPropertyValue(_config.getAxisLabelX(), this, "Label override", "X axis");
	PropertyHelper::setBoolPropertyValue(_config.getXLabelAxisAutoDetermine(), this, "Automatic label", "X axis");
	
	PropertyHelper::setStringPropertyValue(_config.getAxisLabelY(), this, "Label override", "Y axis");
	PropertyHelper::setBoolPropertyValue(_config.getYLabelAxisAutoDetermine(), this, "Automatic label", "Y axis");
}



