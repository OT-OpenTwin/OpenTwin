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

#include "EntityResult1DPlot.h"
#include "PropertyHelper.h"
#include "DataBase.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/VisualisationTypes.h"
#include <algorithm>
#include "OTCore/EntityName.h"
#include "EntityResult1DCurve.h"

static EntityFactoryRegistrar<EntityResult1DPlot> registrar("EntityResult1DPlot_New");

EntityResult1DPlot::EntityResult1DPlot(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	:EntityContainer(_ID,_parent,_obs,_ms)
{}

void EntityResult1DPlot::storeToDataBase(void)
{
	EntityContainer::storeToDataBase();
}

void EntityResult1DPlot::addVisualizationNodes(void)
{

	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "Plot1DVisible";
	treeIcons.hiddenIcon = "Plot1DHidden";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

	ot::VisualisationTypes visTypes;
	visTypes.addPlot1DVisualisation();

	visTypes.addToJsonObject(doc, doc.GetAllocator());
	treeIcons.addToJsonDoc(doc);

	getObserver()->sendMessageToViewer(doc);

	for (auto child : getChildrenList())
	{
		child->addVisualizationNodes();
	}
}

bool EntityResult1DPlot::updateFromProperties(void)
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

bool EntityResult1DPlot::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;
	
	EntityPropertiesSelection* plotType = PropertyHelper::getSelectionProperty(this, "Plot type");
	EntityPropertiesSelection* plotQuantity = PropertyHelper::getSelectionProperty(this, "Plot quantity"); 
	if (plotQuantity->getVisible() != (plotType->getValue() != "Polar - Complex")) {
		plotQuantity->setVisible(plotType->getValue() != "Polar - Complex");
		plotQuantity->resetNeedsUpdate();
		updatePropertiesGrid = true;
	}

	EntityPropertiesBoolean* gridVisibility = PropertyHelper::getBoolProperty(this, "Grid"); 
	EntityPropertiesColor* gridColor = PropertyHelper::getColourProperty(this,"Grid color");

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

void EntityResult1DPlot::createProperties(void)
{
	//Query options are set in the addChild and removeChild methods
	EntityPropertiesSelection::createProperty("Curve set", "X axis parameter", {}, "", "default", getProperties());
	std::list<std::string> allQueryOptions{ "" };
	m_querySettings.setQueryDefinitions(allQueryOptions);
	m_querySettings.setProperties(this);

	//EntityPropertiesSelection::createProperty("General", "Plot type", { "Cartesian", "Polar", "Polar - Complex" }, "Cartesian", "", getProperties());
	EntityPropertiesSelection::createProperty("General", "Plot type", { "Cartesian"}, "Cartesian", "", getProperties());
	EntityPropertiesSelection::createProperty("General", "Plot quantity", { "Magnitude", "Phase", "Real", "Imaginary" }, "Real", "", getProperties());
	EntityPropertiesBoolean::createProperty("General", "Grid", true, "", getProperties());
	EntityPropertiesColor::createProperty("General", "Grid color", { 100, 100, 100 }, "", getProperties());
	EntityPropertiesBoolean::createProperty("General", "Legend", true, "", getProperties());

	EntityPropertiesBoolean::createProperty("X axis", "Logscale", false, "", getProperties());
	EntityPropertiesBoolean::createProperty("X axis", "Autoscale", true, "", getProperties());
	EntityPropertiesDouble::createProperty("X axis", "Min", 0.0, "", getProperties());
	EntityPropertiesDouble::createProperty("X axis", "Max", 0.0, "", getProperties());
	EntityPropertiesBoolean::createProperty("X axis", "Automatic label", true, "", getProperties());
	EntityPropertiesString::createProperty("X axis", "Label override", "", "", getProperties());

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


const ot::Plot1DCfg EntityResult1DPlot::getPlot()
{
	
	const ot::Color gridColour = PropertyHelper::getColourPropertyValue(this,"Grid color");
	
	const std::string entityName = getName();
	auto shortName = ot::EntityName::getSubName(entityName);
	std::string title ("");
	if (shortName.has_value())
	{
		title = shortName.value();
	}
	else
	{
		assert(false);
	}

	const std::string plotType = PropertyHelper::getSelectionPropertyValue(this, "Plot type");
	const std::string plotQuantity = PropertyHelper::getSelectionPropertyValue(this, "Plot quantity");

	const bool gridVisible = PropertyHelper::getBoolPropertyValue(this, "Grid");
	const bool legendVisible = PropertyHelper::getBoolPropertyValue(this, "Legend");
	
	const bool logScaleX = PropertyHelper::getBoolPropertyValue(this, "Logscale", "X axis");
	const bool autoScaleX = PropertyHelper::getBoolPropertyValue(this, "Autoscale", "X axis");
	const double minX = PropertyHelper::getDoublePropertyValue(this, "Min", "X axis");
	const double maxX = PropertyHelper::getDoublePropertyValue(this, "Max", "X axis");

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

	std::list<ValueComparisionDefinition> queries = m_querySettings.getValueComparisionDefinitions(this);

	ot::Plot1DCfg config;
	config.setEntityName(getName());
	config.setEntityID(getEntityID());
	config.setEntityVersion(getEntityStorageVersion());

	config.setTitle(title);
	config.setCollectionName(DataBase::instance().getCollectionName());
	config.setOldTreeIcons(ot::NavigationTreeItemIcon("Plot1DVisible", "Plot1DHidden"));

	config.setXAxisParameter(xAxisParameter);

	config.setPlotType(ot::Plot1DCfg::stringToPlotType(plotType));
	config.setAxisQuantity(ot::Plot1DCfg::stringToAxisQuantity(plotQuantity));

	config.setGridColor(gridColour);
	config.setGridVisible(gridVisible);
	config.setLegendVisible(legendVisible);

	config.setXAxisIsLogScale(logScaleX);
	config.setXAxisIsAutoScale(autoScaleX);
	config.setXAxisMin(minX);
	config.setXAxisMax(maxX);
	
	config.setYAxisIsLogScale(logScaleY);
	config.setYAxisIsAutoScale(autoScaleY);
	config.setYAxisMin(minY);
	config.setYAxisMax(maxY);

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
				filterOptions.push_back(parameterDescr.m_label);
				parameterOptions.push_back(parameterDescr.m_label);
			}
		}
	}
	filterOptions.push_back("");
	filterOptions.sort();
	filterOptions.unique();
	parameterOptions.sort();
	parameterOptions.unique();

	PropertyHelper::getSelectionProperty(this, "X axis parameter", "Curve set")->resetOptions(parameterOptions);
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
	PropertyHelper::setColourPropertyValue(_config.getGridColor(), this, "Grid color");
	PropertyHelper::setSelectionPropertyValue(ot::Plot1DCfg::plotTypeToString(_config.getPlotType()), this, "Plot type");
	PropertyHelper::setSelectionPropertyValue(ot::Plot1DCfg::axisQuantityToString(_config.getAxisQuantity()), this, "Plot quantity");
	
	PropertyHelper::setBoolPropertyValue(_config.getGridVisible(), this, "Grid");
	PropertyHelper::setBoolPropertyValue(_config.getLegendVisible(), this, "Legend");

	PropertyHelper::setBoolPropertyValue(_config.getXAxisIsLogScale(), this, "Logscale", "X axis");
	PropertyHelper::setBoolPropertyValue(_config.getXAxisIsAutoScale(), this, "Autoscale", "X axis");
	PropertyHelper::setDoublePropertyValue(_config.getXAxisMin(), this, "Min", "X axis");
	PropertyHelper::setDoublePropertyValue(_config.getXAxisMax(), this, "Max", "X axis");

	PropertyHelper::setBoolPropertyValue(_config.getYAxisIsLogScale(), this, "Logscale", "Y axis");
	PropertyHelper::setBoolPropertyValue(_config.getYAxisIsAutoScale(), this, "Autoscale", "Y axis");
	PropertyHelper::setDoublePropertyValue(_config.getYAxisMin(), this, "Min", "Y axis");
	PropertyHelper::setDoublePropertyValue(_config.getYAxisMax(), this, "Max", "Y axis");

	PropertyHelper::setStringPropertyValue(_config.getAxisLabelX(), this, "Label override", "X axis");
	PropertyHelper::setBoolPropertyValue(_config.getXLabelAxisAutoDetermine(), this, "Automatic label", "X axis");
	
	PropertyHelper::setStringPropertyValue(_config.getAxisLabelY(), this, "Label override", "Y axis");
	PropertyHelper::setBoolPropertyValue(_config.getYLabelAxisAutoDetermine(), this, "Automatic label", "Y axis");
}



