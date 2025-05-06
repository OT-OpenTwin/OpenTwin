#include "EntityResult1DPlot_New.h"
#include "PropertyHelper.h"
#include "DataBase.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/VisualisationTypes.h"
#include <algorithm>

EntityResult1DPlot_New::EntityResult1DPlot_New(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, ClassFactoryHandler* _factory, const std::string& _owner)
	:EntityContainer(_ID,_parent,_obs,_ms,_factory,_owner)
{}

void EntityResult1DPlot_New::StoreToDataBase(void)
{
	EntityContainer::StoreToDataBase();
}

void EntityResult1DPlot_New::addVisualizationNodes(void)
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

bool EntityResult1DPlot_New::updateFromProperties(void)
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
	
	getObserver()->requestVisualisation(getEntityID(), OT_ACTION_PARAM_VIEW1D_Setup, true, requiresDataToBeFetched);

	getProperties().forceResetUpdateForAllProperties();

	return updatePropertyVisibilities();
}

bool EntityResult1DPlot_New::updatePropertyVisibilities(void)
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

void EntityResult1DPlot_New::createProperties(void)
{
	EntityPropertiesString::createProperty("General", "Title", "", "", getProperties());
	EntityPropertiesSelection::createProperty("General", "Plot type", { "Cartesian", "Polar", "Polar - Complex" }, "Cartesian", "", getProperties());
	EntityPropertiesSelection::createProperty("General", "Plot quantity", { "Magnitude", "Phase", "Real", "Imaginary" }, "Real", "", getProperties());
	EntityPropertiesBoolean::createProperty("General", "Grid", true, "", getProperties());
	EntityPropertiesColor::createProperty("General", "Grid color", { 100, 100, 100 }, "", getProperties());
	EntityPropertiesBoolean::createProperty("General", "Legend", true, "", getProperties());

	EntityPropertiesBoolean::createProperty("X axis", "Logscale", false, "", getProperties());
	EntityPropertiesBoolean::createProperty("X axis", "Autoscale", true, "", getProperties());
	EntityPropertiesDouble::createProperty("X axis", "Min", 0.0, "", getProperties());
	EntityPropertiesDouble::createProperty("X axis", "Max", 0.0, "", getProperties());

	EntityPropertiesBoolean::createProperty("Y axis", "Logscale", false, "", getProperties());
	EntityPropertiesBoolean::createProperty("Y axis", "Autoscale", true, "", getProperties());
	EntityPropertiesDouble::createProperty("Y axis", "Min", 0.0, "", getProperties());
	EntityPropertiesDouble::createProperty("Y axis", "Max", 0.0, "", getProperties());

	EntityPropertiesBoolean::createProperty("Curve limit", "Number of curves", true, "default", getProperties());
	EntityPropertiesInteger::createProperty("Curve limit", "Max", 25, "default", getProperties());

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

void EntityResult1DPlot_New::setFamilyOfCurveProperties(std::list<std::string>& _parameterNames, std::list<std::string>& _quantityNames)
{
	EntityPropertiesSelection::createProperty("Curve set", "X axis parameter", _parameterNames, *_parameterNames.begin(), "default", getProperties());
	
	std::list<std::string> allQueryOptions{""};
	std::merge(_parameterNames.begin(), _parameterNames.end(), _quantityNames.begin(), _quantityNames.end(), std::back_inserter(allQueryOptions));
	
	m_querySettings.setQueryDefinitions(allQueryOptions);
	m_querySettings.setProperties(this);

	getProperties().forceResetUpdateForAllProperties();
}

const ot::Plot1DCfg EntityResult1DPlot_New::getPlot()
{
	int gridColorR = 0, gridColorG = 0, gridColorB = 0;

	const ot::Color gridColour = PropertyHelper::getColourPropertyValue(this,"Grid color");
	const std::string title = PropertyHelper::getStringPropertyValue(this, "Title");
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

	std::list<ValueComparisionDefinition> queries = m_querySettings.getValueComparisionDefinitions(this);

	ot::Plot1DCfg config;
	config.setEntityName(getName());
	config.setEntityID(getEntityID());
	config.setEntityVersion(getEntityStorageVersion());

	config.setTitle(title);
	config.setProjectName(DataBase::GetDataBase()->getProjectName());
	config.setOldTreeIcons(ot::NavigationTreeItemIcon("Plot1DVisible", "Plot1DHidden"));

	config.setXAxisParameter(xAxisParameter);

	config.setPlotType(ot::Plot1DCfg::stringToPlotType(plotType));
	config.setAxisQuantity(ot::Plot1DCfg::stringToAxisQuantity(plotQuantity));

	config.setGridColor(ot::Color(gridColorR, gridColorG, gridColorB));
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

	config.setQueries(queries);

	config.setLimitOfCurves(maxNbOfCurves);
	config.setUseLimitNbOfCurves(useCurveLimit);

	return config;
}

bool EntityResult1DPlot_New::visualisePlot()
{
	return true;
}

void EntityResult1DPlot_New::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityContainer::AddStorageData(storage);
}

void EntityResult1DPlot_New::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);
	m_querySettings.reload(this);
	resetModified();
}

void EntityResult1DPlot_New::setPlot(const ot::Plot1DCfg& _config)
{
	PropertyHelper::setColourPropertyValue(_config.getGridColor(), this, "Grid color");
	PropertyHelper::setStringPropertyValue(_config.getTitle(), this, "Title");
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
}



