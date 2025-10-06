//! @file PropertyManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/String.h"
#include "OTCore/ContainerHelper.h"
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
#include "OTGui/PropertyManagerNotifier.h"
#include "OTGui/PropertyManagerIterator.h"
#include "OTGui/PropertyReadCallbackNotifier.h"
#include "OTGui/PropertyWriteCallbackNotifier.h"

ot::PropertyManager::PropertyManager() : m_silenced(false) {}

ot::PropertyManager::PropertyManager(const ConstJsonObject& _jsonObject) : m_silenced(false) {
	this->setFromJsonObject(_jsonObject);
}

ot::PropertyManager::PropertyManager(PropertyManager&& _other) noexcept :
	m_silenced(std::move(_other.m_silenced)), m_groups(std::move(_other.m_groups)), m_notifier(std::move(_other.m_notifier)),
	m_readNotifier(std::move(_other.m_readNotifier)), m_writeNotifier(std::move(_other.m_writeNotifier))
{

}

ot::PropertyManager::~PropertyManager() {
	this->clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Operator

ot::PropertyManager& ot::PropertyManager::operator=(PropertyManager&& _other) noexcept {
	if (this != &_other) {
		m_silenced = std::move(_other.m_silenced);
		m_groups = std::move(_other.m_groups);
		m_notifier = std::move(_other.m_notifier);
		m_readNotifier = std::move(_other.m_readNotifier);
		m_writeNotifier = std::move(_other.m_writeNotifier);
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

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

// Event handling

void ot::PropertyManager::propertyChanged(const Property* _property) {
	if (m_silenced) {
		return;
	}
	const auto it = m_writeNotifier.find(_property->getPropertyPath());
	if (it != m_writeNotifier.end()) {
		it->second->call(_property);
	}

	for (PropertyManagerNotifier* n : m_notifier) {
		n->propertyHasChanged(_property);
	}
}

void ot::PropertyManager::readingProperty(const std::string& _propertyGroupName, const std::string& _propertyName) const {
	if (m_silenced) {
		return;
	}
	const auto it = m_readNotifier.find(_propertyGroupName + "/" + _propertyName);
	if (it != m_readNotifier.end()) {
		it->second->call(_propertyGroupName, _propertyName);
	}
}

void ot::PropertyManager::notifyAllChanged(void) {
	std::list<Property*> props;
	
	// Get all properties
	for (const auto& it : m_groups) {
		props.splice(props.end(), it.second->getAllProperties());
	}

	// Notify for all
	for (const Property* prop : props) {
		this->propertyChanged(prop);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter

ot::Property* ot::PropertyManager::addProperty(const std::string& _groupName, Property* _property) {
	OTAssertNullptr(_property);
	Property* prop = this->findProperty(_groupName, _property->getPropertyName());
	
	// Adding a property will not update its values
	// This avoids conflicts on multiinitialization
	if (!prop) {
		prop = this->storeProperty(_groupName, _property);
	}
	
	return prop;
}

void ot::PropertyManager::updateProperty(const std::string& _groupName, Property* _property) {
	OTAssertNullptr(_property);

	Property* prop = this->findProperty(_groupName, _property->getPropertyName());
	if (prop) {
		prop->mergeWith(_property, PropertyBase::MergeValues);

		delete _property;
	}
	else {
		prop = this->addProperty(_groupName, _property);
	}

	this->propertyChanged(prop);
}

void ot::PropertyManager::mergeWith(const PropertyManager& _other, const PropertyBase::MergeMode& _mergeMode) {
	for (const auto& newIt : _other.getData()) {
		OTAssertNullptr(newIt.second);

		PropertyGroup* grp = nullptr;

		for (const auto& oldIt : m_groups) {
			if (oldIt.first == newIt.first) {
				OTAssertNullptr(oldIt.second);
				grp = oldIt.second;
				break;
			}
		}

		if (grp) {
			grp->mergeWith(*newIt.second, _mergeMode);
		}
		else if (_mergeMode & Property::AddMissing) {
			// Add copy of whole missing group
			m_groups.insert_or_assign(newIt.first, new PropertyGroup(newIt.second));
		}
	}
}

void ot::PropertyManager::updateProperty(const std::string& _groupName, const Property* _property, bool _deleteProvidedProperty) {
	OTAssertNullptr(_property);

	Property* prop = this->findProperty(_groupName, _property->getPropertyName());
	if (prop) {
		prop->mergeWith(_property, PropertyBase::MergeValues);
	}
	else {
		prop = this->addProperty(_groupName, _property->createCopy());
	}

	if (_deleteProvidedProperty) {
		delete _property;
	}

	this->propertyChanged(prop);
}

ot::PropertyBool* ot::PropertyManager::setBool(const std::string& _groupName, const std::string& _valueName, bool _value) {
	PropertyBool* prop = dynamic_cast<PropertyBool*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	// Update value
	prop->setValue(_value);
	
	// Notify
	this->propertyChanged(prop);

	return prop;
}

ot::PropertyInt* ot::PropertyManager::setInt(const std::string& _groupName, const std::string& _valueName, int _value) {
	PropertyInt* prop = dynamic_cast<PropertyInt*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	// Update value
	prop->setValue(_value);

	// Notify
	this->propertyChanged(prop);

	return prop;
}

ot::PropertyDouble* ot::PropertyManager::setDouble(const std::string& _groupName, const std::string& _valueName, double _value) {
	PropertyDouble* prop = dynamic_cast<PropertyDouble*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	// Update value
	prop->setValue(_value);

	// Notify
	this->propertyChanged(prop);

	return prop;
}

ot::PropertyString* ot::PropertyManager::setString(const std::string& _groupName, const std::string& _valueName, const std::string& _value) {
	PropertyString* prop = dynamic_cast<PropertyString*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	// Update value
	prop->setValue(_value);

	// Notify
	this->propertyChanged(prop);

	return prop;
}

ot::PropertyStringList* ot::PropertyManager::setStringList(const std::string& _groupName, const std::string& _valueName, const std::string& _value) {
	PropertyStringList* prop = dynamic_cast<PropertyStringList*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	// Update value
	prop->setCurrent(_value);

	// Notify
	this->propertyChanged(prop);

	return prop;
}

ot::PropertyColor* ot::PropertyManager::setColor(const std::string& _groupName, const std::string& _valueName, const Color& _value) {
	PropertyColor* prop = dynamic_cast<PropertyColor*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	// Update value
	prop->setValue(_value);

	// Notify
	this->propertyChanged(prop);

	return prop;
}

ot::PropertyPainter2D* ot::PropertyManager::setPainter2D(const std::string& _groupName, const std::string& _valueName, Painter2D* _painter) {
	PropertyPainter2D* prop = dynamic_cast<PropertyPainter2D*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	// Update value
	prop->setPainter(_painter);

	// Notify
	this->propertyChanged(prop);

	return prop;
}

ot::PropertyPainter2D* ot::PropertyManager::setPainter2D(const std::string& _groupName, const std::string& _valueName, const Painter2D* _painter) {
	OTAssertNullptr(_painter);
	return this->setPainter2D(_groupName, _valueName, static_cast<Painter2D*>(_painter->createCopy()));
}

ot::PropertyPath* ot::PropertyManager::setPath(const std::string& _groupName, const std::string& _valueName, const std::string& _path) {
	PropertyPath* prop = dynamic_cast<PropertyPath*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	// Update value
	prop->setPath(_path);

	// Notify
	this->propertyChanged(prop);

	return prop;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Getter

ot::Property* ot::PropertyManager::findProperty(const std::string& _groupName, const std::string& _valueName) {
	ot::PropertyGroup* grp = this->findGroup(_groupName);
	if (grp) {
		return grp->findPropertyByPath(_valueName);
	}
	else {
		return nullptr;
	}
}

const ot::Property* ot::PropertyManager::findProperty(const std::string& _groupName, const std::string& _valueName) const {
	const ot::PropertyGroup* grp = this->findGroup(_groupName);
	if (grp) {
		return grp->findPropertyByPath(_valueName);
	}
	else {
		return nullptr;
	}
}

bool ot::PropertyManager::getBool(const std::string& _groupName, const std::string& _valueName) const {
	this->readingProperty(_groupName, _valueName);

	const PropertyBool* prop = dynamic_cast<const PropertyBool*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);
	
	return prop->getValue();
}

int ot::PropertyManager::getInt(const std::string& _groupName, const std::string& _valueName) const {
	this->readingProperty(_groupName, _valueName);

	const PropertyInt* prop = dynamic_cast<const PropertyInt*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	return prop->getValue();
}

double ot::PropertyManager::getDouble(const std::string& _groupName, const std::string& _valueName) const {
	this->readingProperty(_groupName, _valueName);

	const PropertyDouble* prop = dynamic_cast<const PropertyDouble*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	return prop->getValue();
}

const std::string& ot::PropertyManager::getString(const std::string& _groupName, const std::string& _valueName) const {
	this->readingProperty(_groupName, _valueName);

	const PropertyString* prop = dynamic_cast<const PropertyString*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	return prop->getValue();
}

const ot::Color& ot::PropertyManager::getColor(const std::string& _groupName, const std::string& _valueName) const {
	this->readingProperty(_groupName, _valueName);

	const PropertyColor* prop = dynamic_cast<const PropertyColor*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	return prop->getValue();
}

const ot::Painter2D* ot::PropertyManager::getPainter2D(const std::string& _groupName, const std::string& _valueName) const {
	this->readingProperty(_groupName, _valueName);

	const PropertyPainter2D* prop = dynamic_cast<const PropertyPainter2D*>(this->findProperty(_groupName, _valueName));
	OTAssertNullptr(prop);

	return prop->getPainter();
}

const std::string& ot::PropertyManager::getPath(const std::string& _groupName, const std::string& _valueName) const {
	this->readingProperty(_groupName, _valueName);

	const PropertyPath* prop = dynamic_cast<const PropertyPath*>(this->findProperty(_groupName, _valueName));
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
	PropertyManagerNotifier* noti = _notifier;

	// Find notifier in list
	auto it = std::find(m_notifier.begin(), m_notifier.end(), noti);

	// Check if in list
	if (it == m_notifier.end()) {
		noti = nullptr;
	}
	else {
		// Remove from list
		m_notifier.erase(it);
	}

	// Destroy object if needed
	if (_destroyObject && _notifier) {
		noti = nullptr;
		delete _notifier;
	}

	return noti;
}

void ot::PropertyManager::addReadCallbackNotifier(PropertyReadCallbackNotifier* _notifier) {
	OTAssertNullptr(_notifier);
	OTAssert(m_readNotifier.find(_notifier->getPropertyPath()) == m_readNotifier.end(), "Notifier for given name already stored");
	m_readNotifier.insert_or_assign(_notifier->getPropertyPath(), _notifier);
}

void ot::PropertyManager::removeReadCallbackNotifier(PropertyReadCallbackNotifier* _notifier) {
	OTAssertNullptr(_notifier);
	m_readNotifier.erase(_notifier->getPropertyPath());
}

void ot::PropertyManager::addWriteCallbackNotifier(PropertyWriteCallbackNotifierBase* _notifier) {
	OTAssertNullptr(_notifier);
	OTAssert(m_writeNotifier.find(_notifier->getPropertyPath()) == m_writeNotifier.end(), "Notifier for given name already stored");
	m_writeNotifier.insert_or_assign(_notifier->getPropertyPath(), _notifier);
}

void ot::PropertyManager::removeWriteCallbackNotifier(PropertyWriteCallbackNotifierBase* _notifier) {
	OTAssertNullptr(_notifier);
	m_writeNotifier.erase(_notifier->getPropertyPath());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

ot::Property* ot::PropertyManager::storeProperty(const std::string& _groupName, Property* _property) {
	ot::PropertyGroup* grp = this->findOrCreateGroup(_groupName);
	OTAssertNullptr(grp);
	OTAssert(grp->findPropertyByPath(_property->getPropertyName()) == nullptr, "Property with the given name already exists");

	grp->addProperty(_property);

	return _property;
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
