//! @file PropertyGroup.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/SimpleFactory.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyFactory.h"

ot::PropertyGroup::PropertyGroup()
{
	m_backgroundPainter = new FillPainter2D(Color(48, 48, 48));
}

ot::PropertyGroup::PropertyGroup(const std::string& _name)
	: m_name(_name), m_title(_name)
{
	m_backgroundPainter = new FillPainter2D(Color(48, 48, 48));
}

ot::PropertyGroup::PropertyGroup(const std::string& _name, const std::string& _title)
	: m_name(_name), m_title(_title)
{
	m_backgroundPainter = new FillPainter2D(Color(48, 48, 48));
}

ot::PropertyGroup::~PropertyGroup() {
	auto p = m_properties;
	m_properties.clear();
	for (auto itm : p) delete itm;
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::PropertyGroup::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);

	JsonArray gArr;
	for (PropertyGroup* group : m_childGroups) {
		JsonObject gObj;
		group->addToJsonObject(gObj, _allocator);
		gArr.PushBack(gObj, _allocator);

	}
	_object.AddMember("ChildGroups", gArr, _allocator);

	JsonArray pArr;
	for (Property* prop : m_properties) {
		JsonObject pObj;
		prop->addToJsonObject(pObj, _allocator);
		pArr.PushBack(pObj, _allocator);
	}
	_object.AddMember("Properties", pArr, _allocator);

	JsonObject bObj;
	m_backgroundPainter->addToJsonObject(bObj, _allocator);
	_object.AddMember("BPainter", bObj, _allocator);
}

void ot::PropertyGroup::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");

	ConstJsonArray pArr = json::getArray(_object, "Properties");
	for (JsonSizeType i = 0; i < pArr.Size(); i++) {
		ConstJsonObject pObj = json::getObject(pArr, i);
		Property* p = PropertyFactory::createProperty(pObj);
		if (p) m_properties.push_back(p);
	}

	ConstJsonArray gArr = json::getArray(_object, "ChildGroups");
	for (JsonSizeType i = 0; i < gArr.Size(); i++) {
		ConstJsonObject gObj = json::getObject(gArr, i);
		PropertyGroup* g = new PropertyGroup;
		g->setFromJsonObject(gObj);
		m_childGroups.push_back(g);
	}

	ConstJsonObject bObj = json::getObject(_object, "BPainter");
	Painter2D* newPainter = SimpleFactory::instance().createType<Painter2D>(bObj);
	if (newPainter) { this->setBackgroundPainter(newPainter); }
}

void ot::PropertyGroup::setProperties(const std::list<Property*>& _properties)
{
	m_properties = _properties;
}

void ot::PropertyGroup::addProperty(Property* _property) {
	m_properties.push_back(_property);
}

void ot::PropertyGroup::setChildGroups(const std::list<PropertyGroup*>& _groups) {
	m_childGroups = _groups;
}

void ot::PropertyGroup::addChildGroup(PropertyGroup* _group) {
	m_childGroups.push_back(_group);
}

void ot::PropertyGroup::setBackgroundColor(const Color& _color) {
	this->setBackgroundPainter(new FillPainter2D(_color));
}

void ot::PropertyGroup::setBackgroundPainter(Painter2D* _painter) {
	if (m_backgroundPainter && m_backgroundPainter != _painter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}