
#include <cassert>

#include "EntityPropertiesItems.h"
#include "EntityProperties.h"
#include "EntityContainer.h"
#include "TemplateDefaultManager.h"

#include "OTCore/Logger.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyStringList.h"

void EntityPropertiesBase::setNeedsUpdate(void)
{
	if (container != nullptr) container->setNeedsUpdate(); 
	
	needsUpdateFlag = true; 
}

EntityPropertiesBase::EntityPropertiesBase(const EntityPropertiesBase &other)
{ 
	name = other.name; 
	group = other.group;

	multipleValues = other.multipleValues; 
	container = other.container;
	needsUpdateFlag = other.needsUpdateFlag;
	readOnly = other.readOnly;
	protectedProperty = other.protectedProperty;
	visible = other.visible;
	errorState = other.errorState;
}

void EntityPropertiesBase::copySettings(EntityPropertiesBase *other, EntityBase *root)
{ 
	assert(name == other->getName()); 

	// We keep the container unchanged, since we don't want to overwrite the ownership of this item
	// We also dont assign the group, since the group assignment of a property can not be changed
	multipleValues = other->multipleValues;
	needsUpdateFlag = other->needsUpdateFlag;
	readOnly = other->readOnly;
	protectedProperty = other->protectedProperty;
	visible = other->visible;
	errorState = other->errorState;
}

EntityPropertiesBase& EntityPropertiesBase::operator=(const EntityPropertiesBase &other)
{ 
	if (&other != this) 
	{ 
		// We keep the container unchanged, since we don't want to overwrite the ownership of this item
		name = other.name;
		group = other.group;
		multipleValues = other.multipleValues; 
		needsUpdateFlag = other.needsUpdateFlag;
		readOnly = other.readOnly;
		protectedProperty = other.protectedProperty;
		visible = other.visible;
		errorState = other.errorState;
	}
	
	return *this; 
}

void EntityPropertiesBase::setupPropertyData(ot::PropertyGridCfg& _configuration, ot::Property* _property)
{
	if (this->hasMultipleValues()) _property->setPropertyFlags(_property->propertyFlags() | ot::Property::HasMultipleValues);
	if (this->getReadOnly()) _property->setPropertyFlags(_property->propertyFlags() | ot::Property::IsReadOnly);
	if (!this->getProtected()) _property->setPropertyFlags(_property->propertyFlags() | ot::Property::IsDeletable);
	if (!this->getVisible()) _property->setPropertyFlags(_property->propertyFlags() | ot::Property::IsHidden);
	if (this->getErrorState()) _property->setPropertyFlags(_property->propertyFlags() | ot::Property::HasInputError);

	ot::PropertyGroup* g = _configuration.findOrCreateGroup(this->getGroup());
	OTAssertNullptr(g);
	g->addProperty(_property);
}

void EntityPropertiesDouble::createProperty(const std::string &group, const std::string &name, double defaultValue, const std::string &defaultCategory, EntityProperties &properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	double value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultDouble(defaultCategory, name, defaultValue);

	// Finally create the new property
	properties.createProperty(new EntityPropertiesDouble(name, value), group);
}

void EntityPropertiesDouble::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyDouble* newProp = new ot::PropertyDouble(this->getName(), value);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesDouble::setFromConfiguration(const ot::Property* _property)
{
	const ot::PropertyDouble* actualProperty = dynamic_cast<const ot::PropertyDouble *>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	setValue(actualProperty->value());
}

void EntityPropertiesDouble::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesDouble *entity = dynamic_cast<EntityPropertiesDouble *>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setValue(entity->getValue());
	}
}

void EntityPropertiesInteger::createProperty(const std::string &group, const std::string &name, long defaultValue, const std::string &defaultCategory, EntityProperties &properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	long value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultLong(defaultCategory, name, defaultValue);

	// Finally create the new property
	properties.createProperty(new EntityPropertiesInteger(name, value), group);
}

void EntityPropertiesInteger::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyInt* newProp = new ot::PropertyInt(this->getName(), value);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesInteger::setFromConfiguration(const ot::Property* _property)
{
	const ot::PropertyInt* actualProperty = dynamic_cast<const ot::PropertyInt*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	setValue(actualProperty->value());
}

void EntityPropertiesInteger::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesInteger *entity = dynamic_cast<EntityPropertiesInteger *>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setValue(entity->getValue());
	}
}

void EntityPropertiesBoolean::createProperty(const std::string &group, const std::string &name, bool defaultValue, const std::string &defaultCategory, EntityProperties &properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	bool value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultBool(defaultCategory, name, defaultValue);

	// Finally create the new property
	properties.createProperty(new EntityPropertiesBoolean(name, value), group);
}

void EntityPropertiesBoolean::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyBool* newProp = new ot::PropertyBool(this->getName(), value);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesBoolean::setFromConfiguration(const ot::Property* _property)
{
	const ot::PropertyBool* actualProperty = dynamic_cast<const ot::PropertyBool*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	setValue(actualProperty->value());
}

void EntityPropertiesBoolean::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesBoolean *entity = dynamic_cast<EntityPropertiesBoolean *>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setValue(entity->getValue());
	}
}

void EntityPropertiesString::createProperty(const std::string &group, const std::string &name, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, defaultValue);

	// Finally create the new property
	properties.createProperty(new EntityPropertiesString(name, value), group);
}

void EntityPropertiesString::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyString* newProp = new ot::PropertyString(this->getName(), value);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesString::setFromConfiguration(const ot::Property* _property)
{
	const ot::PropertyString* actualProperty = dynamic_cast<const ot::PropertyString*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	setValue(actualProperty->value());
}

void EntityPropertiesString::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesString *entity = dynamic_cast<EntityPropertiesString *>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setValue(entity->getValue());
	}
}

void EntityPropertiesSelection::createProperty(const std::string &group, const std::string &name, std::list<std::string>& options, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, defaultValue);

	// Finally create the new property
	EntityPropertiesSelection *prop = new EntityPropertiesSelection;
	prop->setName(name);

	for (auto& item : options) prop->addOption(item);

	prop->setValue(value);

	properties.createProperty(prop, group);
}

void EntityPropertiesSelection::createProperty(const std::string& group, const std::string& name, std::list<std::string>&& options, const std::string& defaultValue, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, defaultValue);

	// Finally create the new property
	EntityPropertiesSelection* prop = new EntityPropertiesSelection;
	prop->setName(name);

	for (auto& item : options) prop->addOption(item);

	prop->setValue(value);

	properties.createProperty(prop, group);
}

void EntityPropertiesSelection::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyStringList* newProp = new ot::PropertyStringList(this->getName(), value, options);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesSelection::setFromConfiguration(const ot::Property* _property)
{
	const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	if (value != actualProperty->current()) setNeedsUpdate();
	value = actualProperty->current();

	options.clear();
	for (const std::string& s : actualProperty->list())
	{
		options.push_back(s);
	}
}

void EntityPropertiesSelection::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesSelection *entity = dynamic_cast<EntityPropertiesSelection *>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		assert(checkCompatibilityOfSettings(*entity));
		setValue(entity->getValue());
	}
}

void EntityPropertiesColor::createProperty(const std::string &group, const std::string &name, std::vector<int> defaultValue, const std::string &defaultCategory, EntityProperties &properties)
{
	assert(defaultValue.size() == 3);

	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	int valueR = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor(defaultCategory, name, 0, defaultValue[0]);
	int valueG = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor(defaultCategory, name, 1, defaultValue[1]);
	int valueB = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor(defaultCategory, name, 2, defaultValue[2]);

	// Finally create the new property
	properties.createProperty(new EntityPropertiesColor(name, valueR / 255.0, valueG / 255.0, valueB / 255.0), group);
}

void EntityPropertiesColor::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::Color col((int)(getColorR() * 255.), (int)(getColorG() * 255.), (int)(getColorB() * 255.));
	ot::PropertyColor* newProp = new ot::PropertyColor(this->getName(), col);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesColor::setFromConfiguration(const ot::Property* _property)
{
	const ot::PropertyColor* actualProperty = dynamic_cast<const ot::PropertyColor*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	setColorR(actualProperty->value().r());
	setColorG(actualProperty->value().g());
	setColorB(actualProperty->value().b());
}

EntityPropertiesColor& EntityPropertiesColor::operator=(const EntityPropertiesColor &other)
{
	if (&other != this) 
	{
		EntityPropertiesBase::operator=(other);

		setColorR(other.getColorR());
		setColorG(other.getColorG());
		setColorB(other.getColorB());
	} 
	
	return *this; 
}

void EntityPropertiesColor::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesColor *entity = dynamic_cast<EntityPropertiesColor *>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setColorRGB(entity->getColorR(), entity->getColorG(), entity->getColorB());
	}
}

bool EntityPropertiesBase::needsUpdate(void) 
{ 
	return needsUpdateFlag; 
};

bool EntityPropertiesDouble::hasSameValue(EntityPropertiesBase *other)
{
	EntityPropertiesDouble *entity = dynamic_cast<EntityPropertiesDouble *>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

bool EntityPropertiesInteger::hasSameValue(EntityPropertiesBase *other)
{
	EntityPropertiesInteger *entity = dynamic_cast<EntityPropertiesInteger *>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

bool EntityPropertiesBoolean::hasSameValue(EntityPropertiesBase *other)
{
	EntityPropertiesBoolean *entity = dynamic_cast<EntityPropertiesBoolean *>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

bool EntityPropertiesString::hasSameValue(EntityPropertiesBase *other)
{
	EntityPropertiesString *entity = dynamic_cast<EntityPropertiesString *>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

void EntityPropertiesSelection::resetOptions(std::list<std::string>& _options)
{
	options.clear();
	options.reserve(_options.size());
	for (auto& item : _options) 
	{
		addOption(item);
	}
	if (options.size() == 0)
	{
		value = "";
	}
	else
	{
		value = options.front();
	}
}

bool EntityPropertiesSelection::hasSameValue(EntityPropertiesBase *other)
{
	EntityPropertiesSelection *entity = dynamic_cast<EntityPropertiesSelection *>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

bool EntityPropertiesColor::hasSameValue(EntityPropertiesBase *other)
{
	EntityPropertiesColor *entity = dynamic_cast<EntityPropertiesColor *>(other);

	if (entity == nullptr) return false;

	return (getColorR() == entity->getColorR() && getColorG() == entity->getColorG() && getColorB() == entity->getColorB() );
}

bool EntityPropertiesEntityList::hasSameValue(EntityPropertiesBase *other)
{
	EntityPropertiesEntityList *entity = dynamic_cast<EntityPropertiesEntityList *>(other);

	if (entity == nullptr) return false;

	return (getEntityContainerName() == entity->getEntityContainerName() && getEntityContainerID() == entity->getEntityContainerID() && getValueName() == entity->getValueName() && getValueID() == entity->getValueID());
}

bool EntityPropertiesSelection::setValue(const std::string &s)
{
	if (std::find(options.begin(), options.end(), s) == options.end()) return false; // This value is not a valid option

	if (value != s) setNeedsUpdate();

	value = s;
	return true;
}

bool EntityPropertiesSelection::checkCompatibilityOfSettings(const EntityPropertiesSelection &other)
{
	if (options.size() != other.options.size()) return false;

	for (int i = 0; i < options.size(); i++)
	{
		if (options[i] != other.options[i]) return false;
	}

	return true;
}

bool EntityPropertiesSelection::isCompatible(EntityPropertiesBase *other)
{
	EntityPropertiesSelection *otherItem = dynamic_cast<EntityPropertiesSelection *>(other);
	if (otherItem == nullptr) return false;

	return checkCompatibilityOfSettings(*otherItem);
}

void EntityPropertiesEntityList::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	std::list<std::string> opt;

	if (root != nullptr)
	{
		EntityContainer *container = findContainerFromID(root, getEntityContainerID());
		if (container != nullptr)
		{
			container = findContainerFromName(root, getEntityContainerName());
		}

		if (container != nullptr)
		{
			for (auto child : container->getChildrenList())
			{
				opt.push_back(child->getName());
			}
		}

		EntityBase *entity = findEntityFromID(root, getValueID());

		if (entity != nullptr)
		{
			setValueName(entity->getName());
		}
		else
		{
			entity = findEntityFromName(root, getValueName());
			if (entity != nullptr)
			{
				setValueID(entity->getEntityID());
			}
		}
	}

	if (!root)
	{
		opt.clear();
	}

	ot::JsonDocument dataDoc;
	dataDoc.AddMember("ContainerName", ot::JsonString(this->getEntityContainerName(), dataDoc.GetAllocator()), dataDoc.GetAllocator());
	dataDoc.AddMember("ContainerID", this->getEntityContainerID(), dataDoc.GetAllocator());
	dataDoc.AddMember("ValueName", ot::JsonString(this->getValueName(), dataDoc.GetAllocator()), dataDoc.GetAllocator());
	dataDoc.AddMember("ValueID", this->getValueID(), dataDoc.GetAllocator());

	ot::PropertyStringList* newProp = new ot::PropertyStringList(this->getName(), getValueName(), opt);
	newProp->setSpecialType("EntityList");
	newProp->setAdditionalPropertyData(dataDoc.toJson());
	this->setupPropertyData(_configuration, newProp);
}

EntityContainer *EntityPropertiesEntityList::findContainerFromID(EntityBase *root, ot::UID entityID)
{
	EntityContainer *container = dynamic_cast<EntityContainer *>(root);

	if (container != nullptr)
	{
		if (container->getEntityID() == entityID) return container;

		for (auto child : container->getChildrenList())
		{
			EntityContainer *containerChild = findContainerFromID(child, entityID);
			if (containerChild != nullptr) return containerChild;
		}
	}

	return nullptr;
}

EntityContainer *EntityPropertiesEntityList::findContainerFromName(EntityBase *root, const std::string &entityName)
{
	EntityContainer *container = dynamic_cast<EntityContainer *>(root);

	if (container != nullptr)
	{
		if (container->getName() == entityName) return container;

		for (auto child : container->getChildrenList())
		{
			EntityContainer *containerChild = findContainerFromName(child, entityName);
			if (containerChild != nullptr) return containerChild;
		}
	}

	return nullptr;
}

void EntityPropertiesEntityList::setFromConfiguration(const ot::Property* _property)
{
	const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	if (actualProperty->additionalPropertyData().empty()) {
		OT_LOG_E("Data missing");
		return;
	}

	this->setValueName(actualProperty->current());

	ot::JsonDocument dataDoc;
	dataDoc.fromJson(actualProperty->additionalPropertyData());
	
	this->setEntityContainerName(ot::json::getString(dataDoc, "ContainerName"));
	this->setEntityContainerID(ot::json::getUInt64(dataDoc, "ContainerID"));
	this->setValueID(ot::json::getUInt64(dataDoc, "ValueID"));
}

EntityPropertiesEntityList& EntityPropertiesEntityList::operator=(const EntityPropertiesEntityList &other)
{
	if (&other != this)
	{
		EntityPropertiesBase::operator=(other);

		setEntityContainerName(other.getEntityContainerName()); 
		setEntityContainerID(other.getEntityContainerID()); 
		setValueName(other.getValueName()); 
		setValueID(other.getValueID());
	}

	return *this;
}

void EntityPropertiesEntityList::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	// Here we can either copy the entire settings from another EntityPropertiesEntityList
	// or alternatively from a EntityPropertiesSelection. The latter is required when we read
	// the property change from a UI

	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesEntityList *entity = dynamic_cast<EntityPropertiesEntityList *>(other);
	if (entity != nullptr)
	{
		setEntityContainerName(entity->getEntityContainerName());
		setEntityContainerID(entity->getEntityContainerID());
		setValueName(entity->getValueName());
		setValueID(entity->getValueID());

		return;
	}

	EntityPropertiesSelection *selection = dynamic_cast<EntityPropertiesSelection *>(other);
	assert(selection != nullptr);
	assert(root != nullptr);

	if (selection != nullptr)
	{
		// We keep our current settings for the entity container
		valueName = selection->getValue();

		// Now we need to search for the entity ID for the selected name
		EntityBase *entity = findEntityFromName(root, valueName);
		assert(entity != nullptr);

		if (entity != nullptr)
		{
			setValueID(entity->getEntityID());
		}

		// Now we update the container ID
		EntityContainer *container = findContainerFromID(root, getEntityContainerID());
		if (container != nullptr)
		{
			// The entity id still exists, so update the name if necessary
			setEntityContainerName(container->getName());
		}
		else
		{
			// The container id does not exist, try to find the container by its name
			container = findContainerFromName(root, getEntityContainerName());

			if (container != nullptr)
			{
				setEntityContainerID(container->getEntityID());
			}
		}
	}
}

EntityBase *EntityPropertiesEntityList::findEntityFromName(EntityBase *root, const std::string &entityName)
{
	if (root->getName() == entityName) return root;

	EntityContainer *container = dynamic_cast<EntityContainer *>(root);

	if (container != nullptr)
	{
		for (auto child : container->getChildrenList())
		{
			EntityBase *containerChild = findEntityFromName(child, entityName);
			if (containerChild != nullptr) return containerChild;
		}
	}

	return nullptr;
}

EntityBase *EntityPropertiesEntityList::findEntityFromID(EntityBase *root, ot::UID entityID)
{
	if (root->getEntityID() == entityID) return root;

	EntityContainer *container = dynamic_cast<EntityContainer *>(root);

	if (container != nullptr)
	{
		for (auto child : container->getChildrenList())
		{
			EntityBase *containerChild = findEntityFromID(child, entityID);
			if (containerChild != nullptr) return containerChild;
		}
	}

	return nullptr;
}

void EntityPropertiesEntityList::createProperty(const std::string &group, const std::string &name, const std::string &contName, ot::UID contID, const std::string &valName, ot::UID valID, const std::string &defaultCategory, EntityProperties &properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, valName);

	// Finally create the new property
	properties.createProperty(new EntityPropertiesEntityList(name, contName, contID, value, valID), group);
}


void EntityPropertiesProjectList::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesBase::copySettings(other, root);
	EntityPropertiesSelection* selection = dynamic_cast<EntityPropertiesSelection*>(other);
	_value = selection->getValue();
	setNeedsUpdate();
}

void EntityPropertiesProjectList::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	ot::PropertyStringList* newProp = new ot::PropertyStringList(this->getName(), _value, std::list<std::string>());
	newProp->setSpecialType("ProjectList");
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesProjectList::setFromConfiguration(const ot::Property* _property)
{
	const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	_value = actualProperty->current();
}
