// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "EntityResult1DCurve.h"
#include "EntityResult1DCurveData.h"
#include "DataBase.h"
#include "Types.h"

#include "OTGui/Plot1DCurveInfoCfg.h"
#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityResult1DCurve::EntityResult1DCurve(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityContainer(ID, parent, obs, ms, factory, owner),
	curveData(nullptr),
	curveDataStorageId(-1),
	curveDataStorageVersion(-1),
	color{0.0, 0.0, 0.0},
	dataType {EMPTY}
{
	setCreateVisualizationItem(false);
	setDeletable(false);
}

EntityResult1DCurve::~EntityResult1DCurve()
{
}

bool EntityResult1DCurve::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityResult1DCurve::StoreToDataBase(void)
{
	syncSettingsFromProperties();

	if (curveData != nullptr)
	{
		storeCurveData();
	}

	EntityBase::StoreToDataBase();
}

void EntityResult1DCurve::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityContainer::AddStorageData(storage);

	// Now we store the particular information about the current object
	storage.append(
		bsoncxx::builder::basic::kvp("CurveDataID", curveDataStorageId),
		bsoncxx::builder::basic::kvp("CurveDataVersion", curveDataStorageVersion),
		bsoncxx::builder::basic::kvp("DataType", (long long) dataType),
		bsoncxx::builder::basic::kvp("colorR", color[0]),
		bsoncxx::builder::basic::kvp("colorG", color[1]), 
		bsoncxx::builder::basic::kvp("colorB", color[2]), 
		bsoncxx::builder::basic::kvp("xAxisLabel", xAxisLabel),
		bsoncxx::builder::basic::kvp("yAxisLabel", yAxisLabel),
		bsoncxx::builder::basic::kvp("xAxisUnit", xAxisUnit),
		bsoncxx::builder::basic::kvp("yAxisUnit", yAxisUnit)
	);
}

void EntityResult1DCurve::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	// Here we can load any special information about the entity
	curveDataStorageId = doc_view["CurveDataID"].get_int64();
	curveDataStorageVersion = doc_view["CurveDataVersion"].get_int64();

	dataType = REAL;

	try
	{
		dataType = (tDataType) ((long long) doc_view["DataType"].get_int64());
	}
	catch (std::exception)
	{
	}

	color[0] = doc_view["colorR"].get_double();
	color[1] = doc_view["colorG"].get_double();
	color[2] = doc_view["colorB"].get_double();

	xAxisLabel = doc_view["xAxisLabel"].get_utf8().value.data();
	yAxisLabel = doc_view["yAxisLabel"].get_utf8().value.data();

	xAxisUnit = doc_view["xAxisUnit"].get_utf8().value.data();
	yAxisUnit = doc_view["yAxisUnit"].get_utf8().value.data();

	resetModified();
}

void EntityResult1DCurve::addVisualizationNodes(void)
{
	if (!getName().empty())
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "TextVisible";
		treeIcons.hiddenIcon = "TextHidden";

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

		treeIcons.addToJsonDoc(doc);

		getObserver()->sendMessageToViewer(doc);
	}

	EntityBase::addVisualizationNodes();
}

void EntityResult1DCurve::removeChild(EntityBase *child)
{
	if (child == curveData)
	{
		curveData = nullptr;
	}

	EntityContainer::removeChild(child);
}

void EntityResult1DCurve::createProperties(void)
{
	EntityPropertiesColor::createProperty("General", "Color", { 255, 0, 0 }, "", getProperties());
	EntityPropertiesString::createProperty("X axis", "X axis label", "", "", getProperties());
	EntityPropertiesString::createProperty("X axis", "X axis unit", "", "", getProperties());
	EntityPropertiesString::createProperty("Y axis", "Y axis label", "", "", getProperties());
	EntityPropertiesString::createProperty("Y axis", "Y axis unit", "", "", getProperties());

	getProperties().getProperty("X axis label")->setReadOnly(true);
	getProperties().getProperty("X axis unit")->setReadOnly(true);
	getProperties().getProperty("Y axis label")->setReadOnly(true);
	getProperties().getProperty("Y axis unit")->setReadOnly(true);

	syncSettingsFromProperties();	
	
	getProperties().forceResetUpdateForAllProperties();
}

void EntityResult1DCurve::syncSettingsFromProperties(void)
{
	color[0] = dynamic_cast<EntityPropertiesColor *>(getProperties().getProperty("Color"))->getColorR();
	color[1] = dynamic_cast<EntityPropertiesColor *>(getProperties().getProperty("Color"))->getColorG();
	color[2] = dynamic_cast<EntityPropertiesColor *>(getProperties().getProperty("Color"))->getColorB();

	xAxisLabel = dynamic_cast<EntityPropertiesString *>(getProperties().getProperty("X axis label"))->getValue();
	yAxisLabel = dynamic_cast<EntityPropertiesString *>(getProperties().getProperty("Y axis label"))->getValue();

	xAxisUnit = dynamic_cast<EntityPropertiesString *>(getProperties().getProperty("X axis unit"))->getValue();
	yAxisUnit = dynamic_cast<EntityPropertiesString *>(getProperties().getProperty("Y axis unit"))->getValue();
}

bool EntityResult1DCurve::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Now synchronize the settings and store the result in the data base
	syncSettingsFromProperties();
	StoreToDataBase();

	// Send a notification message to the observer, that the result1d properties have changed
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Result1DPropsChanged, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonArray entities;
	ot::JsonObject curveObj;
	ot::Plot1DCurveInfoCfg curve;
	curve.setId(this->getEntityID());
	curve.setVersion(this->getEntityStorageVersion());
	curve.addToJsonObject(curveObj, doc.GetAllocator());
	entities.PushBack(curveObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_List, entities, doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);

	// We now reset the update flag for all properties, since we took care of this above
	getProperties().forceResetUpdateForAllProperties();

	return false; // Notify, whether property grid update is necessary
}

EntityResult1DCurveData *EntityResult1DCurve::getCurveData(void)
{
	EnsureCurveDataLoaded();
	assert(curveData != nullptr);

	return curveData;
}

void EntityResult1DCurve::deleteCurveData(void)
{
	curveData = nullptr;
	curveDataStorageId = -1;
	curveDataStorageVersion = -1;

	setModified();
}

void EntityResult1DCurve::storeCurveData(void)
{
	if (curveData == nullptr) return;
	assert(curveData != nullptr);

	curveData->StoreToDataBase();

	if (curveDataStorageId != curveData->getEntityID() || curveDataStorageVersion != curveData->getEntityStorageVersion())
	{
		setModified();
	}

	curveDataStorageId = curveData->getEntityID();
	curveDataStorageVersion = curveData->getEntityStorageVersion();
}

void EntityResult1DCurve::releaseCurveData(void)
{
	if (curveData == nullptr) return;

	storeCurveData();

	delete curveData;
	curveData = nullptr;
}

ot::Color EntityResult1DCurve::getColor()
{
	color[0] = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Color"))->getColorR();
	color[1] = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Color"))->getColorG();
	color[2] = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Color"))->getColorB();

	ot::ColorF selectedColor(static_cast<float>(color[0]), static_cast<float>(color[1]), static_cast<float>(color[2]));
	return selectedColor.toColor();
}

std::string EntityResult1DCurve::getAxisLabelX()
{
	xAxisLabel = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("X axis label"))->getValue();
	return xAxisLabel;
}

std::string EntityResult1DCurve::getAxisLabelY()
{
	yAxisLabel = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Y axis label"))->getValue();	
	return yAxisLabel;
}

std::string EntityResult1DCurve::getUnitX()
{
	xAxisUnit = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("X axis unit"))->getValue();
	return xAxisUnit;
}

std::string EntityResult1DCurve::getUnitY()
{
	yAxisUnit = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Y axis unit"))->getValue();
	return yAxisUnit;
}

void EntityResult1DCurve::EnsureCurveDataLoaded(void)
{
	if (curveData == nullptr)
	{
		if (curveDataStorageId == -1)
		{
			curveData = new EntityResult1DCurveData(getUidGenerator()->getUID(), this, getObserver(), getModelState(), getClassFactory(), getOwningService());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			curveData = dynamic_cast<EntityResult1DCurveData *>(readEntityFromEntityID(this, curveDataStorageId, entityMap));
		}

		assert(curveData != nullptr);
		addChild(curveData);
	}
}

void EntityResult1DCurve::setCurveProperty(const std::string &name, const std::string &value)
{
	EntityPropertiesString *prop = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty(name));
	assert(prop != nullptr);

	if (prop)
	{
		prop->setValue(value);
		setModified();
	}
}

void EntityResult1DCurve::setColor(int colorR, int colorG, int colorB)
{
	EntityPropertiesColor *prop = dynamic_cast<EntityPropertiesColor*>(getProperties().getProperty("Color"));
	assert(prop != nullptr);

	if (prop)
	{
		prop->setColorR(colorR / 255.0);
		prop->setColorG(colorG / 255.0);
		prop->setColorB(colorB / 255.0);
		setModified();
	}
}

void EntityResult1DCurve::setCurveXData(const std::vector<double> &x)
{
	getCurveData()->setXData(x);
	setModified();
}

void EntityResult1DCurve::setCurveYData(const std::vector<double> &yre, const std::vector<double> &yim)
{
	if (yre.empty() && yim.empty())
	{
		dataType = EMPTY;
	}
	else if (yim.empty())
	{
		dataType = REAL;
	}
	else if (yre.empty())
	{
		dataType = IMAG;
	}
	else
	{
		dataType = COMPLEX;
	}

	getCurveData()->setYData(yre, yim);
	setModified();
}

void EntityResult1DCurve::setColorFromID(int colorID)
{
	std::vector<int> predefColorsR = { 255,   0,   0, 255, 255, 128,   0,   0, 128, 128 };
	std::vector<int> predefColorsG = {   0, 255,   0,   0, 170,   0, 128,   0,   0,  85 };
	std::vector<int> predefColorsB = {   0,   0, 255, 255,   0,   0,   0, 128, 128,   0 };

	int colorIndex = colorID % predefColorsR.size();

	setColor(predefColorsR[colorIndex], predefColorsG[colorIndex], predefColorsB[colorIndex]);
}


