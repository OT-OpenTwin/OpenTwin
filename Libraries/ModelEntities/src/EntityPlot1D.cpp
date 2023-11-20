// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "EntityPlot1D.h"
#include "DataBase.h"
#include "Types.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityPlot1D::EntityPlot1D(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, factory, owner)
{
	
}

EntityPlot1D::~EntityPlot1D()
{
}

bool EntityPlot1D::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityPlot1D::StoreToDataBase(void)
{
	EntityBase::StoreToDataBase();
}

void EntityPlot1D::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Now we store the particular information about the current objec-t
	auto curveID = bsoncxx::builder::basic::array();
	auto curveName = bsoncxx::builder::basic::array();

	for (auto curve : curves)
	{
		curveID.append((long long) curve);
		curveName.append(curveNames[curve]);
	}

	storage.append(
		bsoncxx::builder::basic::kvp("Curves", curveID),
		bsoncxx::builder::basic::kvp("CurveNames", curveName)
	);
}

void EntityPlot1D::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Here we can load any special information about the entity
	auto arrayCurveItems = doc_view["Curves"].get_array().value;
	auto arrayCurveNames = doc_view["CurveNames"].get_array().value;
	curves.clear();
	curveNames.clear();

	for (bsoncxx::array::element item : arrayCurveItems)
	{
		ot::UID curveID = item.get_int64();
		curves.push_back(curveID);
	}

	auto it = curves.begin();
	for (bsoncxx::array::element name : arrayCurveNames)
	{
		std::string curveName = name.get_utf8().value.data();
		curveNames[*it] = curveName;

		it++;
	}

	resetModified();
}

void EntityPlot1D::addVisualizationNodes(void)
{
	addVisualizationItem(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

void EntityPlot1D::addVisualizationItem(bool isHidden)
{
	TreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "Plot1DVisible";
	treeIcons.hiddenIcon = "Plot1DHidden";

	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_AddPlot1D);
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName, getName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_UID, getEntityID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsHidden, isHidden);
	ot::rJSON::add(doc, OT_ACTION_PARAM_PROJECT_NAME, DataBase::GetDataBase()->getProjectName());

	std::list<unsigned long long> versions;
	for (auto curve : curves)
	{
		versions.push_back(getCurrentEntityVersion(curve));
	}

	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_CurveIDs, curves);
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_CurveVersions, versions);
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_CurveNames, getCurveNames());

	addPropertiesToDocument(doc);

	treeIcons.addToJsonDoc(&doc);

	getObserver()->sendMessageToViewer(doc);
}

void EntityPlot1D::createProperties(void)
{
	EntityPropertiesString::createProperty("General", "Title", "", "", getProperties());
	EntityPropertiesSelection::createProperty("General", "Plot type", { "Cartesian", "Polar", "Polar - Complex" }, "Cartesian", "", getProperties());
	EntityPropertiesSelection::createProperty("General", "Plot quantity", { "Magnitude", "Phase", "Real", "Imaginary" }, "Real", "", getProperties());
	EntityPropertiesBoolean::createProperty("General", "Grid", true, "", getProperties());
	EntityPropertiesColor::createProperty("General", "Grid color", { 100, 100, 100 }, "", getProperties());
	EntityPropertiesBoolean::createProperty("General", "Legend", true, "", getProperties());

	EntityPropertiesBoolean::createProperty("X axis", "Logscale X", false, "", getProperties());
	EntityPropertiesBoolean::createProperty("X axis", "Autoscale X", true, "", getProperties());
	EntityPropertiesDouble::createProperty("X axis", "X min", 0.0, "", getProperties());
	EntityPropertiesDouble::createProperty("X axis", "X max", 0.0, "", getProperties());

	EntityPropertiesBoolean::createProperty("Y axis", "Logscale Y", false, "", getProperties());
	EntityPropertiesBoolean::createProperty("Y axis", "Autoscale Y", true, "", getProperties());
	EntityPropertiesDouble::createProperty("Y axis", "Y min", 0.0, "", getProperties());
	EntityPropertiesDouble::createProperty("Y axis", "Y max", 0.0, "", getProperties());
	
	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityPlot1D::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Send a notification message to the observer, that the result1d properties have changed
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_Plot1DPropsChanged);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_ID, getEntityID());
	addPropertiesToDocument(doc);

	getObserver()->sendMessageToViewer(doc);

	// We now reset the update flag for all properties, since we took care of this above
	getProperties().forceResetUpdateForAllProperties();

	return updatePropertyVisibilities(); // Notify, whether property grid update is necessary
}

void EntityPlot1D::setPlotType(const std::string &type) { 
	if (!getProperties().propertyExists("Plot type")) {
		assert(0);
		return;
	}
	setSelectionPlotProperty("Plot type", type); 
}
std::string EntityPlot1D::getPlotType(void) { 
	if (getProperties().propertyExists("Plot type")) {
		return getSelectionPlotProperty("Plot type");
	}
	else {
		return "Cartesian";
	}
}

void EntityPlot1D::setPlotQuantity(const std::string &quantity) { 
	if (!getProperties().propertyExists("Plot quantity")) {
		assert(0);
		return;
	}
	setSelectionPlotProperty("Plot quantity", quantity);
}
std::string EntityPlot1D::getPlotQuantity(void) { 
	if (getProperties().propertyExists("Plot quantity")) {
		return getSelectionPlotProperty("Plot quantity");
	}
	else {
		return "Real";
	}
}

void EntityPlot1D::addPropertiesToDocument(rapidjson::Document &doc)
{
	int gridColorR = 0, gridColorG = 0, gridColorB = 0;
	getGridColor(gridColorR, gridColorG, gridColorB);

	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_Title, getTitle());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_PlotType, getPlotType());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_PlotQuantity, getPlotQuantity());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_Legend, getLegend());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_Grid, getGrid());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_LogscaleX, getLogscaleX());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_LogscaleY, getLogscaleY());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_AutoscaleX, getAutoscaleX());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_AutoscaleY, getAutoscaleY());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_Xmin, getXmin());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_Xmax, getXmax());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_Ymin, getYmin());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_Ymax, getYmax());
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_GridColorR, gridColorR);
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_GridColorG, gridColorG);
	ot::rJSON::add(doc, OT_ACTION_PARAM_VIEW1D_GridColorB, gridColorB);
}

void EntityPlot1D::setStringPlotProperty(const std::string &name, const std::string &value)
{
	EntityPropertiesString *prop = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	if (prop)
	{
		prop->setValue(value);
		setModified();
	}
}

void EntityPlot1D::setSelectionPlotProperty(const std::string &name, const std::string &value) {
	EntityPropertiesSelection *prop = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	if (prop)
	{
		prop->setValue(value);
		setModified();
	}
}

void EntityPlot1D::setBoolPlotProperty(const std::string &name, bool value)
{
	EntityPropertiesBoolean *prop = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	if (prop)
	{
		prop->setValue(value);
		setModified();
	}
}

void EntityPlot1D::setDoublePlotProperty(const std::string &name, double value)
{
	EntityPropertiesDouble *prop = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	if (prop)
	{
		prop->setValue(value);
		setModified();
	}
}

void EntityPlot1D::setGridColor(int colorR, int colorG, int colorB)
{
	EntityPropertiesColor *prop = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Grid color"));
	assert(prop != nullptr);

	if (prop)
	{
		prop->setColorR(colorR / 255.0);
		prop->setColorG(colorG / 255.0);
		prop->setColorB(colorB / 255.0);
		setModified();
	}
}

std::string EntityPlot1D::getStringPlotProperty(const std::string &name)
{
	EntityPropertiesString *prop = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	return prop->getValue();
}

std::string EntityPlot1D::getSelectionPlotProperty(const std::string &name) {
	EntityPropertiesSelection *prop = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	return prop->getValue();
}

bool EntityPlot1D::getBoolPlotProperty(const std::string &name)
{
	EntityPropertiesBoolean *prop = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	return prop->getValue();
}

double EntityPlot1D::getDoublePlotProperty(const std::string &name)
{
	EntityPropertiesDouble *prop = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	return prop->getValue();
}

void EntityPlot1D::getGridColor(int &colorR, int &colorG, int &colorB)
{
	EntityPropertiesColor *prop = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Grid color"));
	assert(prop != nullptr);

	if (prop)
	{
		colorR = (int)(prop->getColorR() * 255.0 + 0.5);
		colorG = (int)(prop->getColorG() * 255.0 + 0.5);
		colorB = (int)(prop->getColorB() * 255.0 + 0.5);
	}
}

void EntityPlot1D::addCurve(ot::UID curveID, const std::string &name)
{
	deleteCurve(curveID);
	curves.push_back(curveID);
	curveNames[curveID] = name;
}

void EntityPlot1D::deleteCurve(ot::UID curveID)
{
	curves.remove(curveID);
	curveNames.erase(curveID);
}

std::list<ot::UID> EntityPlot1D::getCurves(void)
{
	return curves;
}

std::list<std::string> EntityPlot1D::getCurveNames(void)
{
	std::list<std::string> names;
	for (auto curve : curves)
	{
		names.push_back(curveNames[curve]);
	}

	return names;
}

bool EntityPlot1D::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;

	EntityPropertiesSelection *plotType = dynamic_cast<EntityPropertiesSelection *>(getProperties().getProperty("Plot type"));
	EntityPropertiesSelection *plotQuantity = dynamic_cast<EntityPropertiesSelection *>(getProperties().getProperty("Plot quantity"));
	if (plotQuantity->getVisible() != (plotType->getValue() != "Polar - Complex")) {
		plotQuantity->setVisible(plotType->getValue() != "Polar - Complex");
		plotQuantity->resetNeedsUpdate();
		updatePropertiesGrid = true;
	}

	EntityPropertiesBoolean *gridVisibility = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Grid"));
	EntityPropertiesColor *gridColor = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Grid color"));

	if (gridVisibility->getValue() != gridColor->getVisible())
	{
		gridColor->setVisible(gridVisibility->getValue());
		gridColor->resetNeedsUpdate();
		updatePropertiesGrid = true;
	}

	EntityPropertiesBoolean *autoscaleX = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Autoscale X"));
	EntityPropertiesDouble *minX = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("X min"));
	EntityPropertiesDouble *maxX = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("X max"));
	assert(minX->getValue() == maxX->getValue());

	if (autoscaleX->getValue() == minX->getVisible())
	{
		minX->setVisible(!autoscaleX->getValue());
		maxX->setVisible(!autoscaleX->getValue());
		minX->resetNeedsUpdate();
		maxX->resetNeedsUpdate();
		updatePropertiesGrid = true;
	}

	EntityPropertiesBoolean *autoscaleY = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Autoscale Y"));
	EntityPropertiesDouble *minY = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Y min"));
	EntityPropertiesDouble *maxY = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Y max"));
	assert(minY->getValue() == maxY->getValue());

	if (autoscaleY->getValue() == minY->getVisible())
	{
		minY->setVisible(!autoscaleY->getValue());
		maxY->setVisible(!autoscaleY->getValue());
		minY->resetNeedsUpdate();
		maxY->resetNeedsUpdate();
		updatePropertiesGrid = true;
	}

	return updatePropertiesGrid;
}
