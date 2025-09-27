
#include "EntityProperties.h"

#include "OTCore/LogDispatcher.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/PropertyStringList.h"

#include <cassert>
#include <list>

EntityProperties::~EntityProperties()
{
	deleteAllProperties();
}

void EntityProperties::merge(EntityProperties& other)
{
	for (auto prop : other.m_propertiesList)
	{
		EntityPropertiesBase* property = getProperty(prop->getName(), prop->getGroup());
		if (property == nullptr)
		{
			const std::string key = createKey(prop->getName(), prop->getGroup());
			m_properties[key] = prop;
			m_propertiesList.push_back(prop);
		}
	}
	other.m_properties.clear();
	other.m_propertiesList.clear();
}

void EntityProperties::deleteAllProperties(void)
{
	// Delete all properties
	for (std::map<std::string, EntityPropertiesBase *>::iterator it = m_properties.begin(); it != m_properties.end(); it++)
	{
		delete it->second;
	}

	// Now erase the map and the list
	m_properties.clear();
	m_propertiesList.clear();
}

bool EntityProperties::createProperty(EntityPropertiesBase *prop, const std::string &group, bool addToFront)
{
	EntityPropertiesBase *property = getProperty(prop->getName(),group);

	if (property != nullptr) return false; // Creation failed, because item already exits

	prop->setGroup(group);
	prop->setContainer(this);
	const std::string key = createKey(prop->getName(), group);
	m_properties[key] = prop;

	if (addToFront)
	{
		m_propertiesList.push_front(prop);
	}
	else
	{
		m_propertiesList.push_back(prop);
	}

	return true;
}

bool EntityProperties::updateProperty(EntityPropertiesBase* prop, const std::string& group)
{
	EntityPropertiesBase* property = getProperty(prop->getName(), group);

	if (property != nullptr)
	{
		m_propertiesList.remove(property);
		const std::string key = createKey(prop->getName(), group);
		m_properties.erase(key);
		delete property;
		property = nullptr;
	}

	prop->setGroup(group);
	prop->setContainer(this);

	const std::string key = createKey(prop->getName(), group);
	m_properties[key] = prop;

	m_propertiesList.push_back(prop);

	return true;
}

bool EntityProperties::deleteProperty(const std::string &_name, const std::string& _groupName)
{
	EntityPropertiesBase *property = getProperty(_name, _groupName);

	if (property == nullptr) return false; // Delete failed, because item does not exist

	const std::string key = createKey(_name, _groupName);
	//If the property was created with the group name, but it is deleted without it, this could cause an issue
	m_properties.erase(key);
	m_propertiesList.remove(property);

	delete property;
	property = nullptr;

	return true;
}

EntityPropertiesBase *EntityProperties::getProperty(const std::string& _name, const std::string& _groupName)
{
	if (_groupName == "")
	{
		const bool accesserIsAKey = isKey(_name);
		if(accesserIsAKey)
		{
			auto property = m_properties.find(_name);
			if (property == m_properties.end())
			{
				return nullptr;
			}
			else
			{
				return property->second;
			}
		}
		else
		{
			// Find first of name
			for (auto& property : m_properties)
			{
				const std::string& keyOfCurrentProperty = property.first;
				const std::string name = extractNameFromKey(keyOfCurrentProperty);
				if (name == _name)
				{
					return property.second;
				}
			}
		}
		
		return nullptr;
	}
	else
	{
		const std::string key =	createKey(_name, _groupName);
		auto it = m_properties.find(key);
		if (it == m_properties.end()) {
			return nullptr;
		}
		else {
			return it->second;
		}
	}
}

bool EntityProperties::propertyExists(const std::string &_name, const std::string& _groupName)
{
	EntityPropertiesBase* base = getProperty(_name, _groupName);
	return (base != nullptr);
}

void EntityProperties::checkWhetherUpdateNecessary(void)
{
	m_needsUpdate = false;

	for (std::map<std::string, EntityPropertiesBase *>::iterator it = m_properties.begin(); it != m_properties.end(); it++)
	{
		if (it->second->needsUpdate()) m_needsUpdate = true;
	}
}

void EntityProperties::forceResetUpdateForAllProperties(void)
{
	for (std::map<std::string, EntityPropertiesBase *>::iterator it = m_properties.begin(); it != m_properties.end(); it++)
	{
		it->second->resetNeedsUpdate();
	}

	m_needsUpdate = false;
}

EntityProperties::EntityProperties(const EntityProperties& other)
{
	m_needsUpdate = other.m_needsUpdate;

	for (auto prop : other.m_propertiesList)
	{
		// Add the current property
		EntityPropertiesBase* newProp = prop->createCopy();

		createProperty(newProp, prop->getGroup());
	}
}

EntityProperties& EntityProperties::operator=(const EntityProperties &other)
{
	deleteAllProperties();

	m_needsUpdate = other.m_needsUpdate;

	for (auto prop : other.m_propertiesList)
	{
		// Add the current property
		EntityPropertiesBase *newProp =prop->createCopy();

		createProperty(newProp, prop->getGroup());
	}

	return *this;
}

void EntityProperties::addToConfiguration(EntityBase *root, bool visibleOnly, ot::PropertyGridCfg& _configuration)
{
	// Here we convert the entire container with all its entities into a JSON document
		
	for (auto prop : m_propertiesList)
	{
		if (!visibleOnly || prop->getVisible())
		{
			prop->addToConfiguration(_configuration, root);
		}
	}
}

void EntityProperties::buildFromConfiguration(const ot::PropertyGridCfg& _config, EntityBase* root)
{
	// Here we re-build the current container with the settings in the JSON document. All previous settings will be overridden.

	deleteAllProperties();

	for (const ot::PropertyGroup* g : _config.getRootGroups()) {
		this->buildFromConfiguration(g, root);
	}
}

void EntityProperties::buildFromConfiguration(const ot::PropertyGroup* _groupConfig, EntityBase* root)
{
	for (const ot::Property* p : _groupConfig->getProperties()) {
		EntityPropertiesBase* newSetting(nullptr);

		if (p->getPropertyType() == ot::PropertyBool::propertyTypeString()) newSetting = new EntityPropertiesBoolean;
		else if (p->getPropertyType() == ot::PropertyColor::propertyTypeString()) newSetting = new EntityPropertiesColor;
		else if (p->getPropertyType() == ot::PropertyInt::propertyTypeString()) newSetting = new EntityPropertiesInteger;
		else if (p->getPropertyType() == ot::PropertyDouble::propertyTypeString()) newSetting = new EntityPropertiesDouble;
		else if (p->getPropertyType() == ot::PropertyString::propertyTypeString()) newSetting = new EntityPropertiesString;
		else if (p->getPropertyType() == ot::PropertyPainter2D::propertyTypeString()) newSetting = new EntityPropertiesGuiPainter;
		else if (p->getPropertyType() == ot::PropertyStringList::propertyTypeString()) {
			if (p->getSpecialType() == "EntityList") newSetting = new EntityPropertiesEntityList;
			else if (p->getSpecialType() == "ProjectList") newSetting = new EntityPropertiesProjectList;
			else if (p->getSpecialType().empty()) newSetting = new EntityPropertiesSelection;
			else {
				OT_LOG_E("Unknown string list property special type \"" + p->getSpecialType() + "\"");
				return;
			}
		}
		else {
			OT_LOG_E("Unknown type \"" + p->getPropertyType() + "\"");
			return;
		}

		if (!newSetting) {
			OT_LOG_E("Failed to create property entity");
			return;
		}

		newSetting->setFromConfiguration(p, root);
		this->createProperty(newSetting, _groupConfig->getGroupPath());
	}

	for (const ot::PropertyGroup* childGroup : _groupConfig->getChildGroups()) {
		this->buildFromConfiguration(childGroup, root);
	}
}

std::string EntityProperties::createJSON(EntityBase* root, bool visibleOnly)
{
	// Here we convert the entire container with all its entities into a JSON document

	ot::JsonDocument jsonDoc;

	for (auto prop : m_propertiesList)
	{
		if (!visibleOnly || prop->getVisible())
		{
			ot::JsonObject propObj;
			prop->addToJsonObject(propObj, jsonDoc.GetAllocator(), root);
			jsonDoc.AddMember(ot::JsonString(prop->getName(), jsonDoc.GetAllocator()), propObj, jsonDoc.GetAllocator());
		}
	}

	// Create the output string
	return jsonDoc.toJson();
}

void EntityProperties::buildFromJSON(const std::string& prop, EntityBase* root)
{
	// Here we re-build the current container with the settings in the JSON document. All previous settings will be overridden.

	deleteAllProperties();

	// Read settings into JSOC Doc
	ot::JsonDocument doc;

	// Parse the document with the json string we have "received"
	doc.fromJson(prop);

	// Now loop through all members of the document
	for (ot::JsonMemberIterator i = doc.MemberBegin(); i != doc.MemberEnd(); i++)
	{
		std::string propertyName = i->name.GetString();

		if (i->value.IsObject())
		{
			//const rapidjson::Value &object = i->value;
			std::string type = ot::json::getString(i->value, "Type");
			std::string group;
			if (i->value.HasMember("Group")) {
				group = ot::json::getString(i->value, "Group");
			}

			EntityPropertiesBase* newSetting(nullptr);

			if (type == EntityPropertiesDouble::typeString()) newSetting = new EntityPropertiesDouble;
			else if (type == EntityPropertiesInteger::typeString()) newSetting = new EntityPropertiesInteger;
			else if (type == EntityPropertiesBoolean::typeString()) newSetting = new EntityPropertiesBoolean;
			else if (type == EntityPropertiesString::typeString()) newSetting = new EntityPropertiesString;
			else if (type == EntityPropertiesSelection::typeString()) newSetting = new EntityPropertiesSelection;
			else if (type == EntityPropertiesColor::typeString()) newSetting = new EntityPropertiesColor;
			else if (type == EntityPropertiesEntityList::typeString()) newSetting = new EntityPropertiesEntityList;
			else if (type == EntityPropertiesProjectList::typeString()) newSetting = new EntityPropertiesProjectList;
			else if (type == EntityPropertiesGuiPainter::typeString()) newSetting = new EntityPropertiesGuiPainter;
			else
			{
				OT_LOG_EAS("Unknown property type \"" + type + "\"");
				return;
			}

			if (newSetting) {
				newSetting->readFromJsonObject(i->value.GetObject(), root);
				newSetting->setName(propertyName);

				this->createProperty(newSetting, group);
			}
			else {
				OT_LOG_EA("Failed to create property");
			}
		}
		else
		{
			OT_LOG_E("Unsupported type");
		}
	}
}

void EntityProperties::checkMatchingProperties(EntityProperties &other)
{
	// Here we check whether we have compatible properties to the collection handed over in the argument list.
	// If a property is not compatible, we remove it from the collection

	std::list<std::pair<std::string, std::string>> removeProperties;

	for (std::map<std::string, EntityPropertiesBase *>::const_iterator it = other.m_properties.begin(); it != other.m_properties.end(); it++)
	{
		EntityPropertiesBase *otherProp = it->second;

		EntityPropertiesBase *myProp = getProperty(otherProp->getName(), otherProp->getGroup());

		bool compatible = false;

		if (myProp != nullptr)
		{
			if (otherProp->getType() == myProp->getType())
			{
				if (myProp->isCompatible(otherProp))
				{
					compatible = true;

					// Now check whether the properties have the same value
					if (!myProp->hasSameValue(otherProp))
					{
						otherProp->setHasMultipleValues(true);
					}

					// If one of the common properties is read only, the resulting property is read only
					if (myProp->getReadOnly())
					{
						otherProp->setReadOnly(true);
					}

					// If one of the common properties is protected, the resulting property is protected
					if (myProp->getProtected())
					{
						otherProp->setProtected(true);
					}
				}
			}
		}

		if (!compatible) removeProperties.push_back({ otherProp->getName(), otherProp->getGroup() });
	}

	// Now finally remove the incompatible properties from the other container
	for (auto prop : removeProperties)
	{
		other.deleteProperty(prop.first,prop.second);
	}
}

void EntityProperties::readFromProperties(const EntityProperties &other, EntityBase *root)
{
	// This function sets the properties of our entities from the values in the collection handed over in the argument list

	for (std::map<std::string, EntityPropertiesBase *>::const_iterator it = other.m_properties.begin(); it != other.m_properties.end(); it++)
	{
		EntityPropertiesBase *modified = it->second;

		EntityPropertiesBase *current = getProperty(modified->getName(), modified->getGroup());
		assert(current != nullptr); // We need to have the same property localls

		if (current != nullptr)
		{
			// Check for compatible types
			bool isCompatible =
				modified->getType() == EntityPropertiesBase::SELECTION &&
				(current->getType() == EntityPropertiesBase::ENTITYLIST || current->getType() == EntityPropertiesBase::PROJECTLIST)
				|| modified->getType() == current->getType();
			assert(isCompatible);

			if (current->getReadOnly())
			{
				// The read only item can not be modified. Let's check whether there was an attempt to set another value.
				assert(current->hasSameValue(modified));
			}
			else
			{
				if (!modified->hasMultipleValues())
				{				
					if (isCompatible)
					{
						current->copySettings(modified, root);
					}
				}
			}
		}
	}
}

void EntityProperties::setAllPropertiesReadOnly(void)
{
	for (auto prop : m_properties)
	{
		prop.second->setReadOnly(true);
	}
}

void EntityProperties::setAllPropertiesNonProtected(void)
{
	for (auto prop : m_properties)
	{
		prop.second->setProtected(false);
	}
}

std::list<EntityPropertiesBase*> EntityProperties::getListOfAllProperties(void)
{
	std::list<EntityPropertiesBase*> allProperties;

	for (auto prop : m_properties)
	{
		allProperties.push_back(prop.second);
	}

	return allProperties;
}

std::list<EntityPropertiesBase *> EntityProperties::getListOfPropertiesWhichNeedUpdate(void)
{
	std::list<EntityPropertiesBase *> needsUpdate;

	for (auto prop : m_properties)
	{
		if (prop.second->needsUpdate())
		{
			needsUpdate.push_back(prop.second);
		}
	}

	return needsUpdate;
}

std::list<EntityPropertiesDouble *> EntityProperties::getListOfNumericalProperties(void)
{
	std::list<EntityPropertiesDouble *> numericalProperty;

	for (auto prop : m_properties)
	{
		EntityPropertiesDouble *doubleProp = dynamic_cast<EntityPropertiesDouble *>(prop.second);

		if (doubleProp != nullptr)
		{
			if (!prop.second->getName().empty())
			{
				if (prop.second->getName()[0] == '#')
				{
					numericalProperty.push_back(doubleProp);
				}
			}
		}
	}

	return numericalProperty;
}

std::list<std::string> EntityProperties::getListOfPropertiesForGroup(const std::string &group) const {
	std::list<std::string> propertyList;

	for (auto prop : m_properties)
	{
		if (prop.second->getGroup() == group)
		{
			propertyList.push_back(prop.second->getName());
		}
	}

	return propertyList;
}

std::string EntityProperties::createKey(const std::string& _name, const std::string& _group) {
	return _group + "/" + _name;
}

bool EntityProperties::isKey(const std::string _accesser) const {
	size_t posGroupSeperator = _accesser.find('/');
	return posGroupSeperator != std::string::npos;
}

std::string EntityProperties::extractNameFromKey(const std::string& _key) const {
	size_t posGroupSeperator = _key.find('/');
	const std::string nameOfKey = _key.substr(posGroupSeperator + 1, _key.size());
	return nameOfKey;
}

