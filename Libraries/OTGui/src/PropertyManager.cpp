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
#include "OTGui/PropertyDirectory.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/PropertyStringList.h"
#include "OTGui/PropertyManagerNotifier.h"

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

void ot::PropertyManager::addProperty(const std::string& _groupName, Property* _property) {
	OTAssertNullptr(_property);
	OTAssert(this->findProperty(_groupName, _property->getPropertyName()) == nullptr, "Property already exists");

	this->storeProperty(_groupName, _property);
}

ot::PropertyBool* ot::PropertyManager::setBool(const std::string& _groupName, const std::string& _valueName, bool _value, const Property::PropertyFlags& _flags) {
	PropertyBool* prop = dynamic_cast<PropertyBool*>(this->findProperty(_groupName, _valueName));
	if (prop) {
		// Update value
		prop->setValue(_value);
	}
	else {
		OTAssert(this->findProperty(_groupName, _valueName) == nullptr, "Same property with different type already exists");
		prop = (PropertyBool*) this->storeProperty(_groupName, new PropertyBool(_valueName, _value, _flags));
	}

	this->notifyPropertyChanged(prop);

	return prop;
}

ot::PropertyInt* ot::PropertyManager::setInt(const std::string& _groupName, const std::string& _valueName, int _value, const Property::PropertyFlags& _flags) {
	return this->setInt(_groupName, _valueName, _value, std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max(), _flags);
}

ot::PropertyInt* ot::PropertyManager::setInt(const std::string& _groupName, const std::string& _valueName, int _value, int _min, int _max, const Property::PropertyFlags& _flags) {
	PropertyInt* prop = dynamic_cast<PropertyInt*>(this->findProperty(_groupName, _valueName));
	if (prop) {
		// Update value
		prop->setValue(_value);
	}
	else {
		OTAssert(this->findProperty(_groupName, _valueName) == nullptr, "Same property with different type already exists");
		prop = (PropertyInt*)this->storeProperty(_groupName, new PropertyInt(_valueName, _value, _min, _max, _flags));
	}

	this->notifyPropertyChanged(prop);

	return prop;
}

ot::PropertyDouble* ot::PropertyManager::setDouble(const std::string& _groupName, const std::string& _valueName, double _value, const Property::PropertyFlags& _flags) {
	return this->setDouble(_groupName, _valueName, _value, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), _flags);
}

ot::PropertyDouble* ot::PropertyManager::setDouble(const std::string& _groupName, const std::string& _valueName, double _value, double _min, double _max, const Property::PropertyFlags& _flags) {
	PropertyDouble* prop = dynamic_cast<PropertyDouble*>(this->findProperty(_groupName, _valueName));
	if (prop) {
		// Update value
		prop->setValue(_value);
	}
	else {
		OTAssert(this->findProperty(_groupName, _valueName) == nullptr, "Same property with different type already exists");
		prop = (PropertyDouble*)this->storeProperty(_groupName, new PropertyDouble(_valueName, _value, _min, _max, _flags));
	}

	this->notifyPropertyChanged(prop);

	return prop;
}

ot::PropertyString* ot::PropertyManager::setString(const std::string& _groupName, const std::string& _valueName, const std::string& _value, const Property::PropertyFlags& _flags) {
	PropertyString* prop = dynamic_cast<PropertyString*>(this->findProperty(_groupName, _valueName));
	if (prop) {
		// Update value
		prop->setValue(_value);
	}
	else {
		OTAssert(this->findProperty(_groupName, _valueName) == nullptr, "Same property with different type already exists");
		prop = (PropertyString*)this->storeProperty(_groupName, new PropertyString(_valueName, _value, _flags));
	}

	this->notifyPropertyChanged(prop);

	return prop;
}

ot::PropertyStringList* ot::PropertyManager::setStringList(const std::string& _groupName, const std::string& _valueName, const std::string& _value, const std::list<std::string>& _possibleValues, const Property::PropertyFlags& _flags) {
	PropertyStringList* prop = dynamic_cast<PropertyStringList*>(this->findProperty(_groupName, _valueName));
	if (prop) {
		// Update value
		prop->setCurrent(_value);
		prop->setList(_possibleValues);
	}
	else {
		OTAssert(this->findProperty(_groupName, _valueName) == nullptr, "Same property with different type already exists");
		prop = (PropertyStringList*)this->storeProperty(_groupName, new PropertyStringList(_valueName, _value, _possibleValues, _flags));
	}

	this->notifyPropertyChanged(prop);

	return prop;
}

ot::PropertyColor* ot::PropertyManager::setColor(const std::string& _groupName, const std::string& _valueName, const Color& _value, const Property::PropertyFlags& _flags) {
	PropertyColor* prop = dynamic_cast<PropertyColor*>(this->findProperty(_groupName, _valueName));
	if (prop) {
		// Update value
		prop->setValue(_value);
	}
	else {
		OTAssert(this->findProperty(_groupName, _valueName) == nullptr, "Same property with different type already exists");
		prop = (PropertyColor*)this->storeProperty(_groupName, new PropertyColor(_valueName, _value, _flags));
	}

	this->notifyPropertyChanged(prop);

	return prop;
}

ot::PropertyPainter2D* ot::PropertyManager::setPainter2D(const std::string& _groupName, const std::string& _valueName, Painter2D* _painter, const Property::PropertyFlags& _flags) {
	PropertyPainter2D* prop = dynamic_cast<PropertyPainter2D*>(this->findProperty(_groupName, _valueName));
	if (prop) {
		// Update value
		prop->setPainter(_painter);
	}
	else {
		OTAssert(this->findProperty(_groupName, _valueName) == nullptr, "Same property with different type already exists");
		prop = (PropertyPainter2D*)this->storeProperty(_groupName, new PropertyPainter2D(_valueName, _painter, _flags));
	}

	this->notifyPropertyChanged(prop);

	return prop;
}

ot::PropertyPainter2D* ot::PropertyManager::setPainter2D(const std::string& _groupName, const std::string& _valueName, const Painter2D* _painter, const Property::PropertyFlags& _flags) {
	OTAssertNullptr(_painter);
	return this->setPainter2D(_groupName, _valueName, static_cast<Painter2D*>(_painter->createCopy()));
}

ot::PropertyFilePath* ot::PropertyManager::setFilePath(const std::string& _groupName, const std::string& _valueName, const std::string& _path, PropertyFilePath::BrowseMode _browseMode, const Property::PropertyFlags& _flags) {
	PropertyFilePath* prop = dynamic_cast<PropertyFilePath*>(this->findProperty(_groupName, _valueName));
	if (prop) {
		// Update value
		prop->setPath(_path);
	}
	else {
		OTAssert(this->findProperty(_groupName, _valueName) == nullptr, "Same property with different type already exists");
		prop = (PropertyFilePath*)this->storeProperty(_groupName, new PropertyFilePath(_valueName, _path, _browseMode, _flags));
	}

	this->notifyPropertyChanged(prop);

	return prop;
}

ot::PropertyDirectory* ot::PropertyManager::setDirectory(const std::string& _groupName, const std::string& _valueName, const std::string& _path, const Property::PropertyFlags& _flags) {
	PropertyDirectory* prop = dynamic_cast<PropertyDirectory*>(this->findProperty(_groupName, _valueName));
	if (prop) {
		// Update value
		prop->setPath(_path);
	}
	else {
		OTAssert(this->findProperty(_groupName, _valueName) == nullptr, "Same property with different type already exists");
		prop = (PropertyDirectory*)this->storeProperty(_groupName, new PropertyDirectory(_valueName, _path, _flags));
	}

	this->notifyPropertyChanged(prop);

	return prop;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Getter

ot::Property* ot::PropertyManager::findProperty(const std::string& _groupName, const std::string& _valueName) const {
	const ot::PropertyGroup* grp = this->findGroup(_groupName);
	if (grp) {
		return grp->findPropertyByPath(_valueName);
	}
	else {
		return nullptr;
	}
}

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

const std::string& ot::PropertyManager::getString(const std::string& _groupName, const std::string& _valueName) const {
	const PropertyGroup* grp = this->findGroup(_groupName);
	OTAssertNullptr(grp);
	const PropertyString* prop = dynamic_cast<const PropertyString*>(grp->findPropertyByPath(_valueName));
	OTAssertNullptr(prop);
	return prop->getValue();
}

const ot::Color& ot::PropertyManager::getColor(const std::string& _groupName, const std::string& _valueName) const {
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

const std::string& ot::PropertyManager::getFilePath(const std::string& _groupName, const std::string& _valueName) const {
	const PropertyGroup* grp = this->findGroup(_groupName);
	OTAssertNullptr(grp);
	const PropertyFilePath* prop = dynamic_cast<const PropertyFilePath*>(grp->findPropertyByPath(_valueName));
	OTAssertNullptr(prop);
	return prop->getPath();
}

const std::string& ot::PropertyManager::getDirectory(const std::string& _groupName, const std::string& _valueName) const {
	const PropertyGroup* grp = this->findGroup(_groupName);
	OTAssertNullptr(grp);
	const PropertyDirectory* prop = dynamic_cast<const PropertyDirectory*>(grp->findPropertyByPath(_valueName));
	OTAssertNullptr(prop);
	return prop->getPath();
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

void ot::PropertyManager::addNotifier(PropertyManagerNotifier* _notifier) {
	OTAssertNullptr(_notifier);
	OTAssert(std::find(m_notifier.begin(), m_notifier.end(), _notifier) == m_notifier.end(), "Notifier already stored");
	_notifier->m_manager = this;
	m_notifier.push_back(_notifier);
}

ot::PropertyManagerNotifier* ot::PropertyManager::removeNotifier(PropertyManagerNotifier* _notifier, bool _destroyObject) {
	PropertyManagerNotifier* noti = nullptr;

	auto it = std::find(m_notifier.begin(), m_notifier.end(), _notifier);
	if (it != m_notifier.end()) {
		noti = *it;
		if (_destroyObject && noti) {
			delete noti;
			noti = nullptr;
		}
	}
	return noti;
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

ot::Property* ot::PropertyManager::storeProperty(const std::string& _groupName, Property* _property) {
	ot::PropertyGroup* grp = this->findOrCreateGroup(_groupName);
	OTAssertNullptr(grp);
	OTAssert(grp->findPropertyByPath(_property->getPropertyName()) == nullptr, "Property with the given name already exists");

	grp->addProperty(_property);
	
	this->notifyPropertyCreated(_property);

	return _property;
}

void ot::PropertyManager::notifyPropertyCreated(Property* _property) {
	OTAssertNullptr(_property);
	for (PropertyManagerNotifier* n : m_notifier) {
		n->propertyCreated(_property);
	}
}

void ot::PropertyManager::notifyPropertyDestroyed(Property* _property) {
	OTAssertNullptr(_property);
	for (PropertyManagerNotifier* n : m_notifier) {
		n->propertyDestroyed(_property);
	}
}

void ot::PropertyManager::notifyPropertyChanged(const Property* _property) {
	OTAssertNullptr(_property);
	for (PropertyManagerNotifier* n : m_notifier) {
		n->propertyChanged(_property);
	}
}
