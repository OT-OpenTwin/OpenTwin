// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "EntityResult1DPlot.h"
#include "DataBase.h"
#include "Types.h"

#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityResult1DPlot::EntityResult1DPlot(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityContainer(ID, parent, obs, ms, factory, owner)
{
	
}

EntityResult1DPlot::~EntityResult1DPlot()
{
}

bool EntityResult1DPlot::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityResult1DPlot::StoreToDataBase(void)
{
	EntityContainer::StoreToDataBase();
}

void EntityResult1DPlot::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityContainer::AddStorageData(storage);

	// Now we store the particular information about the current objec-t
	auto curveID = bsoncxx::builder::basic::array();
	auto curveName = bsoncxx::builder::basic::array();

	for (const ot::Plot1DCurveInfoCfg& curve : m_curves)
	{
		curveID.append((long long) curve.getId());
		curveName.append(curve.getName());
	}

	storage.append(
		bsoncxx::builder::basic::kvp("Curves", curveID),
		bsoncxx::builder::basic::kvp("CurveNames", curveName),
		bsoncxx::builder::basic::kvp("ServiceName", m_serviceInfo.serviceName()),
		bsoncxx::builder::basic::kvp("ServiceType", m_serviceInfo.serviceType())
	);
}

void EntityResult1DPlot::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	// Here we can load any special information about the entity
	auto arrayCurveItems = doc_view["Curves"].get_array().value;
	auto arrayCurveNames = doc_view["CurveNames"].get_array().value;
	try {
		m_serviceInfo.setServiceName(doc_view["ServiceName"].get_utf8().value.data());
		m_serviceInfo.setServiceType(doc_view["ServiceType"].get_utf8().value.data());
	}
	catch (...) {}
	m_curves.clear();

	for (const bsoncxx::array::element& item : arrayCurveItems)
	{
		ot::UID curveID = item.get_int64();
		m_curves.push_back(ot::Plot1DCurveInfoCfg(curveID, 0, ""));
	}

	auto it = m_curves.begin();
	for (const bsoncxx::array::element& name : arrayCurveNames)
	{
		OTAssert(it != m_curves.end(), "Size mismatch");
		it->setName(name.get_utf8().value.data());
		it++;
	}
	OTAssert(it == m_curves.end(), "Size mismatch");

	resetModified();
}

void EntityResult1DPlot::addVisualizationNodes(void)
{
	addVisualizationItem(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

void EntityResult1DPlot::addVisualizationItem(bool _isHidden)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddPlot1D, doc.GetAllocator()), doc.GetAllocator());
	m_serviceInfo.addToJsonObject(doc, doc.GetAllocator());

	ot::Plot1DDataBaseCfg plotCfg;
	this->addToConfiguration(plotCfg, true);
	plotCfg.setHidden(_isHidden);
	ot::JsonObject cfgObj;
	plotCfg.addToJsonObject(cfgObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
}

void EntityResult1DPlot::createProperties(void)
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

bool EntityResult1DPlot::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Send a notification message to the observer, that the result1d properties have changed
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Plot1DPropsChanged, doc.GetAllocator()), doc.GetAllocator());
	m_serviceInfo.addToJsonObject(doc, doc.GetAllocator());

	ot::Plot1DDataBaseCfg plotCfg;
	this->addToConfiguration(plotCfg, false);
	ot::JsonObject cfgObj;
	plotCfg.addToJsonObject(cfgObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);

	// We now reset the update flag for all properties, since we took care of this above
	getProperties().forceResetUpdateForAllProperties();

	return updatePropertyVisibilities(); // Notify, whether property grid update is necessary
}

void EntityResult1DPlot::setPlotType(ot::Plot1DCfg::PlotType _type) {
	if (!getProperties().propertyExists("Plot type")) {
		OT_LOG_EAS("Property \"Plot type\" does not exist");
		return;
	}
	setSelectionPlotProperty("Plot type", ot::Plot1DCfg::plotTypeToString(_type)); 
}
ot::Plot1DCfg::PlotType EntityResult1DPlot::getPlotType(void) {
	if (getProperties().propertyExists("Plot type")) {
		return ot::Plot1DCfg::stringToPlotType(getSelectionPlotProperty("Plot type"));
	}
	else {
		return ot::Plot1DCfg::Cartesian;
	}
}

void EntityResult1DPlot::setPlotQuantity(ot::Plot1DCfg::AxisQuantity _quantity) {
	if (!getProperties().propertyExists("Plot quantity")) {
		OT_LOG_EAS("Property \"Plot quantity\" does not exist");
		return;
	}
	setSelectionPlotProperty("Plot quantity", ot::Plot1DCfg::axisQuantityToString(_quantity));
}
ot::Plot1DCfg::AxisQuantity EntityResult1DPlot::getPlotQuantity(void) {
	if (getProperties().propertyExists("Plot quantity")) {
		return ot::Plot1DCfg::stringToAxisQuantity(getSelectionPlotProperty("Plot quantity"));
	}
	else {
		return ot::Plot1DCfg::Real;
	}
}

void EntityResult1DPlot::setStringPlotProperty(const std::string &name, const std::string &value)
{
	EntityPropertiesString *prop = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	if (prop)
	{
		prop->setValue(value);
		setModified();
	}
}

void EntityResult1DPlot::setSelectionPlotProperty(const std::string &name, const std::string &value) {
	EntityPropertiesSelection *prop = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	if (prop)
	{
		prop->setValue(value);
		setModified();
	}
}

void EntityResult1DPlot::setBoolPlotProperty(const std::string &name, bool value)
{
	EntityPropertiesBoolean *prop = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	if (prop)
	{
		prop->setValue(value);
		setModified();
	}
}

void EntityResult1DPlot::setDoublePlotProperty(const std::string &name, double value)
{
	EntityPropertiesDouble *prop = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	if (prop)
	{
		prop->setValue(value);
		setModified();
	}
}

void EntityResult1DPlot::setGridColor(int colorR, int colorG, int colorB)
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

std::string EntityResult1DPlot::getStringPlotProperty(const std::string &name)
{
	EntityPropertiesString *prop = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	return prop->getValue();
}

std::string EntityResult1DPlot::getSelectionPlotProperty(const std::string &name) {
	EntityPropertiesSelection *prop = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	return prop->getValue();
}

bool EntityResult1DPlot::getBoolPlotProperty(const std::string &name)
{
	EntityPropertiesBoolean *prop = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	return prop->getValue();
}

double EntityResult1DPlot::getDoublePlotProperty(const std::string &name)
{
	EntityPropertiesDouble *prop = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	return prop->getValue();
}

void EntityResult1DPlot::getGridColor(int &colorR, int &colorG, int &colorB)
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

void EntityResult1DPlot::addCurve(ot::UID _curveID, const std::string& _name)
{
	this->deleteCurve(_curveID);
	m_curves.push_back(ot::Plot1DCurveInfoCfg(_curveID, 0, _name));
	this->setModified();
}

void EntityResult1DPlot::deleteCurve(ot::UID _curveID)
{
	bool erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_curves.begin(); it != m_curves.end(); it++) {
			if (it->getId() == _curveID) {
				m_curves.erase(it);
				erased = true;
				setModified();
				break;
			}
		}
	}
}

void EntityResult1DPlot::deleteCurve(const std::string& _curveName)
{
	bool erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_curves.begin(); it != m_curves.end(); it++) {
			if (it->getName() == _curveName) {
				m_curves.erase(it);
				erased = true;
				setModified();
				break;
			}
		}
	}
}

void EntityResult1DPlot::overrideReferencedCurves(const std::list<ot::Plot1DCurveInfoCfg>& _curves)
{
	m_curves = _curves;
	setModified();
}

bool EntityResult1DPlot::updatePropertyVisibilities(void)
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

void EntityResult1DPlot::updateCurveVersions(void) {
	for (ot::Plot1DCurveInfoCfg& curve : m_curves) {
		curve.setVersion(this->getCurrentEntityVersion(curve.getId()));
	}
}

void EntityResult1DPlot::addToConfiguration(ot::Plot1DDataBaseCfg& _plotCfg, bool _includeCurves) {
	_plotCfg.setTreeIcons(ot::NavigationTreeItemIcon("Plot1DVisible", "Plot1DHidden"));
	_plotCfg.setProjectName(DataBase::GetDataBase()->getProjectName());
	_plotCfg.setUid(this->getEntityID());
	_plotCfg.setName(this->getName());
	_plotCfg.setTitle(this->getTitle());
	_plotCfg.setPlotType(this->getPlotType());
	_plotCfg.setAxisQuantity(this->getPlotQuantity());
	_plotCfg.setLegendVisible(this->getLegendVisible());
	_plotCfg.setGridVisible(this->getGridVisible());

	int r = 0, g = 0, b = 0;
	this->getGridColor(r, g, b);
	_plotCfg.setGridColor(ot::Color(r, g, b));

	_plotCfg.setXAxisMin(this->getXmin());
	_plotCfg.setXAxisMax(this->getXmax());
	_plotCfg.setXAxisIsLogScale(this->getLogscaleX());
	_plotCfg.setXAxisIsAutoScale(this->getAutoscaleX());

	_plotCfg.setYAxisMin(this->getYmin());
	_plotCfg.setYAxisMax(this->getYmax());
	_plotCfg.setYAxisIsLogScale(this->getLogscaleY());
	_plotCfg.setYAxisIsAutoScale(this->getAutoscaleY());

	if (_includeCurves) {
		this->updateCurveVersions();
		_plotCfg.setCurves(m_curves);
	}
}
