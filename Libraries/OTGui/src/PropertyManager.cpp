//! @file PropertyManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTGui/Painter2D.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/PropertyManager.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/PropertyStringList.h"

ot::PropertyManager::PropertyManager() {}

ot::PropertyManager::PropertyManager(const PropertyManager& _other) {
	for (const auto& it : _other.m_groups) {
		m_groups.insert_or_assign(it.first, it.second->createCopy(true));
	}
}

ot::PropertyManager::PropertyManager(PropertyManager&& _other) noexcept :
	m_groups(std::move(_other.m_groups))
{

}

ot::PropertyManager::~PropertyManager() {
	this->clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Operator

ot::PropertyManager& ot::PropertyManager::operator=(const PropertyManager& _other) {
	if (this != &_other) {
		this->clear();

		// Copy groups
		for (const auto& it : _other.m_groups) {
			m_groups.insert_or_assign(it.first, it.second->createCopy(true));
		}
	}

	return *this;
}

ot::PropertyManager& ot::PropertyManager::operator=(PropertyManager&& _other) noexcept {
	if (this != &_other) {
		m_groups = std::move(_other.m_groups);
	}

	return *this;
}

void ot::PropertyManager::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonArray arr;
	for (const auto& it : m_groups) {
		JsonObject obj;
		it.second->addToJsonObject(obj, _allocator);
		arr.PushBack(obj, _allocator);
	}

	_object.AddMember("Data", arr, _allocator);
}

void ot::PropertyManager::setFromJsonObject(const ot::ConstJsonObject& _object) {
	this->clear();
	for (const ConstJsonObject& obj : json::getObjectList(_object, "Data")) {
		PropertyGroup* newGroup = new PropertyGroup;
		newGroup->setFromJsonObject(obj);

		OTAssert(m_groups.find(newGroup->getName()) == m_groups.end(), "Duplicate group name");
		m_groups.insert_or_assign(newGroup->getName(), newGroup);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter

ot::PropertyBool* ot::PropertyManager::setBool(const std::string& _groupName, const std::string& _valueName, bool _value, const Property::PropertyFlags& _flags) {
	return (PropertyBool*) this->storeProperty(_groupName, new PropertyBool(_valueName, _value, _flags));
}

ot::PropertyInt* ot::PropertyManager::setInt(const std::string& _groupName, const std::string& _valueName, int _value, const Property::PropertyFlags& _flags) {
	return this->setInt(_groupName, _valueName, _value, std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max(), _flags);
}

ot::PropertyInt* ot::PropertyManager::setInt(const std::string& _groupName, const std::string& _valueName, int _value, int _min, int _max, const Property::PropertyFlags& _flags) {
	return (PropertyInt*)this->storeProperty(_groupName, new PropertyInt(_valueName, _value, _min, _max, _flags));
}

ot::PropertyDouble* ot::PropertyManager::setDouble(const std::string& _groupName, const std::string& _valueName, double _value, const Property::PropertyFlags& _flags) {
	return this->setDouble(_groupName, _valueName, _value, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), _flags);
}

ot::PropertyDouble* ot::PropertyManager::setDouble(const std::string& _groupName, const std::string& _valueName, double _value, double _min, double _max, const Property::PropertyFlags& _flags) {
	return (PropertyDouble*)this->storeProperty(_groupName, new PropertyDouble(_valueName, _value, _min, _max, _flags));
}

ot::PropertyString* ot::PropertyManager::setString(const std::string& _groupName, const std::string& _valueName, const std::string& _value, const Property::PropertyFlags& _flags) {
	return (PropertyString*)this->storeProperty(_groupName, new PropertyString(_valueName, _value, _flags));
}

ot::PropertyStringList* ot::PropertyManager::setStringList(const std::string& _groupName, const std::string& _valueName, const std::string& _value, const std::list<std::string>& _possibleValues, const Property::PropertyFlags& _flags) {
	return (PropertyStringList*)this->storeProperty(_groupName, new PropertyStringList(_valueName, _value, _possibleValues, _flags));
}

ot::PropertyColor* ot::PropertyManager::setColor(const std::string& _groupName, const std::string& _valueName, const Color& _value, const Property::PropertyFlags& _flags) {
	return (PropertyColor*)this->storeProperty(_groupName, new PropertyColor(_valueName, _value, _flags));
}

ot::PropertyPainter2D* ot::PropertyManager::setPainter2D(const std::string& _groupName, const std::string& _valueName, Painter2D* _painter, const Property::PropertyFlags& _flags) {
	OTAssertNullptr(_painter);
	return (PropertyPainter2D*)this->storeProperty(_groupName, new PropertyPainter2D(_valueName, _painter, _flags));
}

ot::PropertyPainter2D* ot::PropertyManager::setPainter2D(const std::string& _groupName, const std::string& _valueName, const Painter2D* _painter, const Property::PropertyFlags& _flags) {
	OTAssertNullptr(_painter);
	return this->setPainter2D(_groupName, _valueName, static_cast<Painter2D*>(_painter->createCopy()));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Getter

bool ot::PropertyManager::getBool(const std::string& _groupName, const std::string& _valueName) const {
	const PropertyGroup* grp = this->findGroup(_groupName);
	OTAssertNullptr(grp);
	const PropertyBool* prop = dynamic_cast<const PropertyBool*>(grp->findPropertyByPath(_valueName));
	OTAssertNullptr(prop);
	return prop->getValue();
}

int ot::PropertyManager::getInt(const std::string& _groupName, const std::string& _valueName) const {
	const PropertyGroup* grp = this->findGroup(_groupName);
	OTAssertNullptr(grp);
	const PropertyInt* prop = dynamic_cast<const PropertyInt*>(grp->findPropertyByPath(_valueName));
	OTAssertNullptr(prop);
	return prop->getValue();
}

double ot::PropertyManager::getDouble(const std::string& _groupName, const std::string& _valueName) const {
	const PropertyGroup* grp = this->findGroup(_groupName);
	OTAssertNullptr(grp);
	const PropertyDouble* prop = dynamic_cast<const PropertyDouble*>(grp->findPropertyByPath(_valueName));
	OTAssertNullptr(prop);
	return prop->getValue();
}

std::string ot::PropertyManager::getString(const std::string& _groupName, const std::string& _valueName) const {
	const PropertyGroup* grp = this->findGroup(_groupName);
	OTAssertNullptr(grp);
	const PropertyString* prop = dynamic_cast<const PropertyString*>(grp->findPropertyByPath(_valueName));
	OTAssertNullptr(prop);
	return prop->getValue();
}

ot::Color ot::PropertyManager::getColor(const std::string& _groupName, const std::string& _valueName) const {
	const PropertyGroup* grp = this->findGroup(_groupName);
	OTAssertNullptr(grp);
	const PropertyColor* prop = dynamic_cast<const PropertyColor*>(grp->findPropertyByPath(_valueName));
	OTAssertNullptr(prop);
	return prop->getValue();
}

const ot::Painter2D* ot::PropertyManager::getPainter2D(const std::string& _groupName, const std::string& _valueName) const {
	const PropertyGroup* grp = this->findGroup(_groupName);
	OTAssertNullptr(grp);
	const PropertyPainter2D* prop = dynamic_cast<const PropertyPainter2D*>(grp->findPropertyByPath(_valueName));
	OTAssertNullptr(prop);
	return prop->getPainter();
}

ot::PropertyGridCfg ot::PropertyManager::createGridConfiguration(void) const {
	PropertyGridCfg cfg;

	for (const auto& it : m_groups) {
		cfg.addRootGroup(it.second->createCopy(true));
	}

	return cfg;
}

void ot::PropertyManager::clear(void) {
	for (const auto& it : m_groups) {
		delete it.second;
	}
	m_groups.clear();
}

ot::PropertyGroup* ot::PropertyManager::findGroup(const std::string& _group) const {
	auto it = m_groups.find(_group);
	if (it != m_groups.end()) {
		return it->second;
	}
	else {
		return nullptr;
	}
}

ot::PropertyGroup* ot::PropertyManager::findOrCreateGroup(const std::string& _group) {
	PropertyGroup* grp = this->findGroup(_group);
	if (!grp) {
		grp = new PropertyGroup(_group);
		m_groups.insert_or_assign(_group, grp);
	}

	return grp;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

ot::Property* ot::PropertyManager::findProperty(const std::string& _groupName, const std::string& _valueName) const {
	const ot::PropertyGroup* grp = this->findGroup(_groupName);
	if (grp) {
		return grp->findPropertyByPath(_valueName);
	}
	else {
		OTAssert(0, "Property group not found");
		return nullptr;
	}
}

ot::Property* ot::PropertyManager::storeProperty(const std::string& _groupName, Property* _property) {
	ot::PropertyGroup* grp = this->findOrCreateGroup(_groupName);
	OTAssertNullptr(grp);
	OTAssert(grp->findPropertyByPath(_property->getPropertyName()) != nullptr, "Property with the given name already exists");

	grp->addProperty(_property);
	
	return _property;
}
