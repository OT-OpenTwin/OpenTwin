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
#include "OTCore/Symbol.h"
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
	:EntityContainer(_ID, _parent, _obs, _ms)
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
	requiresDataToBeFetched |= numberOfCurves->needsUpdate();
	auto numberOfCurvesMax = PropertyHelper::getIntegerProperty(this, "Max", "Curve limit");
	requiresDataToBeFetched |= numberOfCurvesMax->needsUpdate();
	requiresDataToBeFetched |= m_querySettings.requiresUpdate(this);

	auto showFullMatrixProp = PropertyHelper::getBoolProperty(this, "Show full matrix");
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
	EntityPropertiesGuiPainter* gridColor = PropertyHelper::getPainterProperty(this, "Grid color");

	if (gridVisibility->getValue() != gridColor->getVisible())
	{
		gridColor->setVisible(gridVisibility->getValue());
		gridColor->resetNeedsUpdate();
		updatePropertiesGrid = true;
	}

	updatePropertiesGrid |= updateAxisPropertiesVisibility(getXAxisPropertyGroupName());
	updatePropertiesGrid |= updateAxisPropertiesVisibility(getYAxisPropertyGroupName());
	updatePropertiesGrid |= updateAxisPropertiesVisibility(getRadiusAxisPropertyGroupName());
	updatePropertiesGrid |= updateAxisPropertiesVisibility(getAzimuthAxisPropertyGroupName());

	ot::Plot1DCfg::PlotType plotType = getPlotType();
	switch (plotType)
	{
	case ot::Plot1DCfg::Cartesian:
		updatePropertiesGrid |= setAxisPropertiesVisibility(getXAxisPropertyGroupName(), true);
		updatePropertiesGrid |= setAxisPropertiesVisibility(getYAxisPropertyGroupName(), true);
		updatePropertiesGrid |= setAxisPropertiesVisibility(getRadiusAxisPropertyGroupName(), false);
		updatePropertiesGrid |= setAxisPropertiesVisibility(getAzimuthAxisPropertyGroupName(), false);
		break;

	case ot::Plot1DCfg::Polar:
		updatePropertiesGrid |= setAxisPropertiesVisibility(getXAxisPropertyGroupName(), false);
		updatePropertiesGrid |= setAxisPropertiesVisibility(getYAxisPropertyGroupName(), false);
		updatePropertiesGrid |= setAxisPropertiesVisibility(getRadiusAxisPropertyGroupName(), true);
		updatePropertiesGrid |= setAxisPropertiesVisibility(getAzimuthAxisPropertyGroupName(), true);
		break;

	default:
		OT_LOG_E("Unknown plot type (" + std::to_string(static_cast<int>(getPlotType())) + ")");
		break;
	}

	updatePropertiesGrid |= m_querySettings.updatePropertyVisibility(this);

	EntityPropertiesDouble* originProp = PropertyHelper::getDoubleProperty(this, "Origin", "General");
	OTAssertNullptr(originProp);
	const bool originPropVis = (plotType == ot::Plot1DCfg::Polar);
	if (originProp->getVisible() != originPropVis)
	{
		originProp->setVisible(originPropVis);
		updatePropertiesGrid = true;
	}

	return updatePropertiesGrid;
}

void EntityResult1DPlot::hideAxisQuantityProperties()
{
	PropertyHelper::getSelectionProperty(this, "Quantity", getXAxisPropertyGroupName())->setVisible(false);
	PropertyHelper::getSelectionProperty(this, "Quantity", getYAxisPropertyGroupName())->setVisible(false);
	PropertyHelper::getSelectionProperty(this, "Quantity", getAzimuthAxisPropertyGroupName())->setVisible(false);
	PropertyHelper::getSelectionProperty(this, "Quantity", getRadiusAxisPropertyGroupName())->setVisible(false);
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
	// Query options are set in the addChild and removeChild methods
	EntityPropertiesSelection::createProperty("Curve set", "X axis parameter", {}, "", "default", getProperties());
	std::list<std::string> allQueryOptions{ "" };
	m_querySettings.setQueryDefinitions(allQueryOptions);
	m_querySettings.setProperties(this);

	EntityPropertiesBoolean::createProperty(m_querySettings.getGroupQuerySettingsName(), "Show full matrix", true, "", getProperties());
	EntityPropertiesInteger::createProperty(m_querySettings.getGroupQuerySettingsName(), "Show matrix row entry", 1, 1, 60, "", getProperties());
	EntityPropertiesInteger::createProperty(m_querySettings.getGroupQuerySettingsName(), "Show matrix column entry", 1, 1, 60, "", getProperties());

	// General settings

	const std::list<std::string> plotTypeOptions = ot::Plot1DCfg::getPlotTypeStringList();
	EntityPropertiesSelection::createProperty("General", "Plot type", plotTypeOptions, ot::Plot1DCfg::toString(ot::Plot1DCfg::Cartesian), "", getProperties());
	EntityPropertiesBoolean::createProperty("General", "Grid", true, "", getProperties());
	EntityPropertiesGuiPainter::createProperty("General", "Grid color", new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::PlotGrid), "", getProperties());
	EntityPropertiesBoolean::createProperty("General", "Legend", true, "", getProperties());
	auto originProp = EntityPropertiesDouble::createProperty("General", "Origin", 0.0, -359.999, 359.999, "", getProperties());
	originProp->setAllowCustomValues(false);
	originProp->setSuffix("deg");
	originProp->setToolTip("Plot origin offset in degrees.");
	originProp->setDecimalPlaces(3);

	// Axis settings

	createAxisProperties(getXAxisPropertyGroupName());
	createAxisProperties(getYAxisPropertyGroupName());
	createAxisProperties(getAzimuthAxisPropertyGroupName());
	createAxisProperties(getRadiusAxisPropertyGroupName());

	// Curve limit settings

	EntityPropertiesBoolean::createProperty("Curve limit", "Number of curves", true, "default", getProperties());
	EntityPropertiesInteger::createProperty("Curve limit", "Max", 25, "default", getProperties());

	updatePropertyVisibilities();
	getProperties().forceResetUpdateForAllProperties();
}

const ot::Plot1DCfg EntityResult1DPlot::getPlot()
{

	const ot::Painter2D* gridColour = PropertyHelper::getPainterPropertyValue(this, "Grid color");

	const std::string entityName = getName();
	auto shortName = ot::EntityName::getSubName(entityName);
	std::string title("");
	if (shortName.has_value())
	{
		title = shortName.value();
	}
	else
	{
		assert(false);
	}

	const std::string plotType = PropertyHelper::getSelectionPropertyValue(this, "Plot type");

	const bool gridVisible = PropertyHelper::getBoolPropertyValue(this, "Grid");
	const bool legendVisible = PropertyHelper::getBoolPropertyValue(this, "Legend");

	const int32_t maxNbOfCurves = PropertyHelper::getIntegerPropertyValue(this, "Max", "Curve limit");
	const bool useCurveLimit = PropertyHelper::getBoolPropertyValue(this, "Number of curves", "Curve limit");

	const std::string xAxisParameter = PropertyHelper::getSelectionPropertyValue(this, "X axis parameter", "Curve set");

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

	config.setShowEntireMatrix(showEntireMatrix);
	config.setShowMatrixColumnEntry(showMatrixColumnValue);
	config.setShowMatrixRowEntry(showMatrixRowValue);

	config.setQueries(queries);

	config.setLimitOfCurves(maxNbOfCurves);
	config.setUseLimitNbOfCurves(useCurveLimit);

	config.setPolarDegreeOrigin(PropertyHelper::getDoublePropertyValue(this, "Origin", "General"));

	// Setup axis

	ot::Plot1DAxisCfg xAxisCfg = config.getXAxis();
	ot::Plot1DAxisCfg yAxisCfg = config.getYAxis();

	switch (config.getPlotType())
	{
	case ot::Plot1DCfg::Cartesian:
		setAxisFromProperties(getXAxisPropertyGroupName(), xAxisCfg);
		setAxisFromProperties(getYAxisPropertyGroupName(), yAxisCfg);
		break;

	case ot::Plot1DCfg::Polar:
		setAxisFromProperties(getRadiusAxisPropertyGroupName(), xAxisCfg);
		setAxisFromProperties(getAzimuthAxisPropertyGroupName(), yAxisCfg);
		break;

	default:
		OT_LOG_E("Unknown plot type (" + std::to_string(static_cast<int>(config.getPlotType())) + ")");
		break;
	}

	config.setXAxis(std::move(xAxisCfg));
	config.setYAxis(std::move(yAxisCfg));

	return config;
}

bool EntityResult1DPlot::visualisePlot()
{
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Property Setter / Getter

ot::Plot1DCfg::PlotType EntityResult1DPlot::getPlotType() const
{
	const std::string plotType = PropertyHelper::getSelectionPropertyValue(this, "Plot type", "General");
	return ot::Plot1DCfg::stringToPlotType(plotType);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private helper

void EntityResult1DPlot::createAxisProperties(const std::string& _axisName)
{
	const std::list<std::string> quantityOptions = ot::Plot1DAxisCfg::getAxisQuantityStringList();
	const std::list<std::string> quantityScalingOptions = ot::Plot1DAxisCfg::getQuantityScalingStringList();
	EntityPropertiesSelection::createProperty(_axisName, "Quantity", quantityOptions, ot::Plot1DAxisCfg::toString(ot::Plot1DAxisCfg::Real), "", getProperties());
	EntityPropertiesSelection::createProperty(_axisName, "Quantity scaling", quantityScalingOptions, ot::Plot1DAxisCfg::toString(ot::Plot1DAxisCfg::NoQuantityScaling), "", getProperties());

	EntityPropertiesBoolean::createProperty(_axisName, "Logscale", false, "", getProperties());
	EntityPropertiesBoolean::createProperty(_axisName, "Autoscale", true, "", getProperties());

	EntityPropertiesDouble::createProperty(_axisName, "Min", 0.0, "", getProperties());
	EntityPropertiesDouble::createProperty(_axisName, "Max", 0.0, "", getProperties());

	EntityPropertiesBoolean::createProperty(_axisName, "Automatic label", true, "", getProperties());
	EntityPropertiesString::createProperty(_axisName, "Label override", "", "", getProperties());

	std::list<std::string> displayNumberFormatOptions = {
		ot::String::toString(ot::String::Auto),
		ot::String::toString(ot::String::Integer),
		ot::String::toString(ot::String::Decimal),
		ot::String::toString(ot::String::Scientific),
		ot::String::toString(ot::String::Engineering)
	};
	EntityPropertiesSelection::createProperty(_axisName, "Display number format", std::move(displayNumberFormatOptions), ot::String::toString(ot::String::Auto), "", getProperties());
	EntityPropertiesInteger::createProperty(_axisName, "Display number precision", 3, 1, 99, "", getProperties())->setAllowCustomValues(false);
}

void EntityResult1DPlot::setAxisFromProperties(const std::string& _axisName, ot::Plot1DAxisCfg& _axis)
{
	const std::string quantity = PropertyHelper::getSelectionPropertyValue(this, "Quantity", _axisName);
	const std::string quantityScaling = PropertyHelper::getSelectionPropertyValue(this, "Quantity scaling", _axisName);

	const bool logScale = PropertyHelper::getBoolPropertyValue(this, "Logscale", _axisName);
	const bool autoScale = PropertyHelper::getBoolPropertyValue(this, "Autoscale", _axisName);

	const double min = PropertyHelper::getDoublePropertyValue(this, "Min", _axisName);
	const double max = PropertyHelper::getDoublePropertyValue(this, "Max", _axisName);

	const bool automaticLabel = PropertyHelper::getBoolPropertyValue(this, "Automatic label", _axisName);
	const std::string labelOverride = PropertyHelper::getStringPropertyValue(this, "Label override", _axisName);

	const ot::String::DisplayNumberFormat displayNumberFormat = ot::String::stringToDisplayNumberFormat(PropertyHelper::getSelectionPropertyValue(this, "Display number format", _axisName));
	const int displayNumberPrecision = PropertyHelper::getIntegerPropertyValue(this, "Display number precision", _axisName);

	_axis.setMin(min);
	_axis.setMax(max);

	_axis.setIsLogScale(logScale);
	_axis.setIsAutoScale(autoScale);

	_axis.setQuantity(ot::Plot1DAxisCfg::stringToAxisQuantity(quantity));
	_axis.setQuantityScaling(ot::Plot1DAxisCfg::stringToQuantityScalingFlag(quantityScaling));

	_axis.setAutoDetermineAxisLabel(automaticLabel);
	_axis.setAxisLabel(labelOverride);

	_axis.setDisplayNumberFormat(displayNumberFormat);
	_axis.setDisplayNumberPrecision(displayNumberPrecision);
}

bool EntityResult1DPlot::setAxisPropertiesVisibility(const std::string& _axisName, bool _visible)
{
	auto selProp = PropertyHelper::getSelectionProperty(this, "Quantity", _axisName);
	OTAssertNullptr(selProp);
	if (selProp->getVisible() == _visible)
	{
		return false;
	}
	selProp->setVisible(_visible);
	getProperties().getProperty("Quantity scaling", _axisName)->setVisible(_visible);
	getProperties().getProperty("Logscale", _axisName)->setVisible(_visible);
	getProperties().getProperty("Autoscale", _axisName)->setVisible(_visible);

	getProperties().getProperty("Min", _axisName)->setVisible(_visible);
	getProperties().getProperty("Max", _axisName)->setVisible(_visible);

	getProperties().getProperty("Automatic label", _axisName)->setVisible(_visible);
	getProperties().getProperty("Label override", _axisName)->setVisible(_visible);

	getProperties().getProperty("Display number format", _axisName)->setVisible(_visible);
	getProperties().getProperty("Display number precision", _axisName)->setVisible(_visible);

	return true;
}

bool EntityResult1DPlot::updateAxisPropertiesVisibility(const std::string& _axisName)
{
	bool changed = false;

	// Min / Max
	EntityPropertiesBoolean* autoscaleProp = PropertyHelper::getBoolProperty(this, "Autoscale", _axisName);
	EntityPropertiesDouble* minProp = PropertyHelper::getDoubleProperty(this, "Min", _axisName);
	EntityPropertiesDouble* maxProp = PropertyHelper::getDoubleProperty(this, "Max", _axisName);

	const bool minMaxVisible = !autoscaleProp->getValue();

	if (minProp->getVisible() != minMaxVisible)
	{
		minProp->setVisible(minMaxVisible);
		minProp->resetNeedsUpdate();

		maxProp->setVisible(minMaxVisible);
		maxProp->resetNeedsUpdate();

		changed = true;
	}

	// Label
	EntityPropertiesBoolean* autoLabelProp = PropertyHelper::getBoolProperty(this, "Automatic label", _axisName);
	EntityPropertiesString* labelOverrideProp = PropertyHelper::getStringProperty(this, "Label override", _axisName);
	if (autoLabelProp->getValue() == labelOverrideProp->getVisible())
	{
		labelOverrideProp->setVisible(!autoLabelProp->getValue());
		labelOverrideProp->resetNeedsUpdate();
		changed = true;
	}

	// Display number format
	const ot::String::DisplayNumberFormat displayNumberFormat = ot::String::stringToDisplayNumberFormat(PropertyHelper::getSelectionPropertyValue(this, "Display number format", _axisName));
	bool precisionVisible = displayNumberFormat != ot::String::DisplayNumberFormat::Integer;
	auto displayNumberPrecisionProp = PropertyHelper::getIntegerProperty(this, "Display number precision", _axisName);
	OTAssertNullptr(displayNumberPrecisionProp);
	if (precisionVisible != displayNumberPrecisionProp->getVisible())
	{
		displayNumberPrecisionProp->setVisible(precisionVisible);
		displayNumberPrecisionProp->resetNeedsUpdate();
		changed = true;
	}

	return changed;
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
			filterOptions.push_back(queryInformation.getQuantityDescription().getLabel());
			for (const auto& parameterDescr : queryInformation.getParameterDescriptions())
			{
				if (find(filterOptions.begin(), filterOptions.end(), parameterDescr.getLabel()) == filterOptions.end())
				{
					filterOptions.push_back(parameterDescr.getLabel());
				}
				if (find(parameterOptions.begin(), parameterOptions.end(), parameterDescr.getLabel()) == parameterOptions.end())
				{
					parameterOptions.push_back(parameterDescr.getLabel());
				}
			}
		}
	}
	if (std::find(filterOptions.begin(), filterOptions.end(), "") == filterOptions.end())
	{
		filterOptions.push_back("");
	}

	PropertyHelper::getSelectionProperty(this, "X axis parameter", "Curve set")->resetOptions(parameterOptions);
	if (parameterOptions.size() != 0)
	{
		PropertyHelper::getSelectionProperty(this, "X axis parameter", "Curve set")->setValue(*parameterOptions.begin());
	}
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
	std::string xAxisPropGroup;
	std::string yAxisPropGroup;

	switch (getPlotType())
	{
	case ot::Plot1DCfg::Cartesian:
		xAxisPropGroup = getXAxisPropertyGroupName();
		yAxisPropGroup = getYAxisPropertyGroupName();
		break;

	case ot::Plot1DCfg::Polar:
		xAxisPropGroup = getRadiusAxisPropertyGroupName();
		yAxisPropGroup = getAzimuthAxisPropertyGroupName();
		break;

	default:
		OT_LOG_E("Unknown plot type (" + std::to_string(static_cast<int>(getPlotType())) + ")");
		break;
	}

	PropertyHelper::setPainterPropertyValue(_config.getGridColor(), this, "Grid color");
	PropertyHelper::setSelectionPropertyValue(ot::Plot1DCfg::toString(_config.getPlotType()), this, "Plot type");

	PropertyHelper::setBoolPropertyValue(_config.getGridVisible(), this, "Grid");
	PropertyHelper::setBoolPropertyValue(_config.getLegendVisible(), this, "Legend");
	PropertyHelper::setDoublePropertyValue(_config.getPolarDegreeOrigin(), this, "Origin", "General");

	PropertyHelper::setSelectionPropertyValue(ot::Plot1DAxisCfg::toString(_config.getXAxisQuantity()), this, "Quantity", xAxisPropGroup);
	PropertyHelper::setBoolPropertyValue(_config.getXAxisIsLogScale(), this, "Logscale", xAxisPropGroup);
	PropertyHelper::setBoolPropertyValue(_config.getXAxisIsAutoScale(), this, "Autoscale", xAxisPropGroup);
	PropertyHelper::setDoublePropertyValue(_config.getXAxisMin(), this, "Min", xAxisPropGroup);
	PropertyHelper::setDoublePropertyValue(_config.getXAxisMax(), this, "Max", xAxisPropGroup);
	PropertyHelper::setStringPropertyValue(_config.getYAxisLabel(), this, "Label override", xAxisPropGroup);
	PropertyHelper::setBoolPropertyValue(_config.getXAxisLabelAutoDetermine(), this, "Automatic label", xAxisPropGroup);

	PropertyHelper::setSelectionPropertyValue(ot::Plot1DAxisCfg::toString(_config.getYAxisQuantity()), this, "Quantity", yAxisPropGroup);
	PropertyHelper::setBoolPropertyValue(_config.getYAxisIsLogScale(), this, "Logscale", yAxisPropGroup);
	PropertyHelper::setBoolPropertyValue(_config.getYAxisIsAutoScale(), this, "Autoscale", yAxisPropGroup);
	PropertyHelper::setDoublePropertyValue(_config.getYAxisMin(), this, "Min", yAxisPropGroup);
	PropertyHelper::setDoublePropertyValue(_config.getYAxisMax(), this, "Max", yAxisPropGroup);
	PropertyHelper::setStringPropertyValue(_config.getXAxisLabel(), this, "Label override", yAxisPropGroup);
	PropertyHelper::setBoolPropertyValue(_config.getYAxisLabelAutoDetermine(), this, "Automatic label", yAxisPropGroup);

	updatePropertyVisibilities();
	updateAxisPropertiesVisibility(xAxisPropGroup);
	updateAxisPropertiesVisibility(yAxisPropGroup);
}



