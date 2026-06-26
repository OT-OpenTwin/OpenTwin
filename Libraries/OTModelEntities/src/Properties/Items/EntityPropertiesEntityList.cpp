// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Properties/PropertyStringList.h"
#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/EntityContainer.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesEntityList.h"

bool EntityPropertiesEntityList::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesEntityList* entity = dynamic_cast<EntityPropertiesEntityList*>(other);

	if (entity == nullptr) return false;

	return (getEntityContainerName() == entity->getEntityContainerName()
		&& getEntityContainerID() == entity->getEntityContainerID()
		&& getValueName() == entity->getValueName()
		&& getValueID() == entity->getValueID()
		&& getFilter() == entity->getFilter()
		&& getIncludeRoot() == entity->getIncludeRoot()
		&& getRecursive() == entity->getRecursive());
}

void EntityPropertiesEntityList::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	std::list<std::string> opt;

	if (root != nullptr)
	{
		this->updateValueAndContainer(root, opt);
	}

	if (!root)
	{
		opt.clear();
	}

	ot::JsonDocument dataDoc;
	dataDoc.AddMember("ContainerName", ot::JsonString(this->getEntityContainerName(), dataDoc.GetAllocator()), dataDoc.GetAllocator());
	dataDoc.AddMember("ContainerID", this->getEntityContainerID(), dataDoc.GetAllocator());
	dataDoc.AddMember("ValueID", this->getValueID(), dataDoc.GetAllocator());
	dataDoc.AddMember("Filter", ot::JsonString(this->getFilter(), dataDoc.GetAllocator()), dataDoc.GetAllocator());
	dataDoc.AddMember("IncludeRoot", this->getIncludeRoot(), dataDoc.GetAllocator());
	dataDoc.AddMember("Recursive", this->getRecursive(), dataDoc.GetAllocator());

	ot::PropertyStringList* newProp = new ot::PropertyStringList(this->getName(), this->getValueName(), opt);
	newProp->setSpecialType("EntityList");
	newProp->addAdditionalPropertyData("EntityData", dataDoc.toJson());
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesEntityList::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList*>(_property);
	if (!actualProperty)
	{
		OT_LOG_E("Property cast failed");
		return;
	}

	std::string entityData = actualProperty->getAdditionalPropertyData("EntityData");
	if (entityData.empty())
	{
		OT_LOG_E("Data missing");
		return;
	}

	ot::JsonDocument dataDoc;
	dataDoc.fromJson(entityData);

	//this->setValueName(ot::json::getString(dataDoc, "ValueName"));
	this->setValueName(actualProperty->getCurrent());
	this->setEntityContainerName(ot::json::getString(dataDoc, "ContainerName"));
	this->setEntityContainerID(ot::json::getUInt64(dataDoc, "ContainerID"));
	this->setValueID(ot::json::getUInt64(dataDoc, "ValueID"));

	if (dataDoc.HasMember("Filter")) this->setFilter(ot::json::getString(dataDoc, "Filter"));
	if (dataDoc.HasMember("IncludeRoot"))this->setIncludeRoot(ot::json::getBool(dataDoc, "IncludeRoot"));
	if (dataDoc.HasMember("Recursive")) this->setRecursive(ot::json::getBool(dataDoc, "Recursive"));

	if (_root)
	{
		std::list<std::string> opt;
		this->updateValueAndContainer(_root, opt);
	}

}

void EntityPropertiesEntityList::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);

	std::list<std::string> opt;

	if (_root)
	{
		this->updateValueAndContainer(_root, opt);
	}

	_jsonObject.AddMember("ContainerName", ot::JsonString(this->getEntityContainerName(), _allocator), _allocator);
	_jsonObject.AddMember("ContainerID", static_cast<int64_t>(this->getEntityContainerID()), _allocator);
	_jsonObject.AddMember("ValueName", ot::JsonString(this->getValueName(), _allocator), _allocator);
	_jsonObject.AddMember("ValueID", static_cast<int64_t>(this->getValueID()), _allocator);
	_jsonObject.AddMember("Filter", ot::JsonString(this->getFilter(), _allocator), _allocator);
	_jsonObject.AddMember("IncludeRoot", this->getIncludeRoot(), _allocator);
	_jsonObject.AddMember("Recursive", this->getRecursive(), _allocator);

	if (_root)
	{
		_jsonObject.AddMember("Options", ot::JsonArray(opt, _allocator), _allocator);
	}
}

void EntityPropertiesEntityList::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);

	const rapidjson::Value& containerName = _object["ContainerName"];
	const rapidjson::Value& containerID = _object["ContainerID"];
	const rapidjson::Value& valName = _object["ValueName"];
	const rapidjson::Value& valID = _object["ValueID"];

	this->setEntityContainerName(containerName.GetString());
	this->setEntityContainerID(containerID.GetInt64());
	this->setValueName(valName.GetString());
	this->setValueID(valID.GetInt64());
	this->setFilter("");
	this->setIncludeRoot(false);
	this->setRecursive(false);

	if (_object.HasMember("Filter")) this->setFilter(_object["Filter"].GetString());
	if (_object.HasMember("IncludeRoot")) this->setIncludeRoot(_object["IncludeRoot"].GetBool());
	if (_object.HasMember("Recursive")) this->setRecursive(_object["Recursive"].GetBool());

	if (_root)
	{
		std::list<std::string> opt;
		this->updateValueAndContainer(_root, opt);
	}
}

EntityPropertiesEntityList& EntityPropertiesEntityList::operator=(const EntityPropertiesEntityList& other)
{
	if (&other != this)
	{
		EntityPropertiesBase::operator=(other);

		setEntityContainerName(other.getEntityContainerName());
		setEntityContainerID(other.getEntityContainerID());
		setValueName(other.getValueName());
		setValueID(other.getValueID());
		setFilter(other.getFilter());
		setIncludeRoot(other.getIncludeRoot());
		setRecursive(other.getRecursive());
	}

	return *this;
}

void EntityPropertiesEntityList::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	// Here we can either copy the entire settings from another EntityPropertiesEntityList
	// or alternatively from a EntityPropertiesSelection. The latter is required when we read
	// the property change from a UI

	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesEntityList* entity = dynamic_cast<EntityPropertiesEntityList*>(other);
	if (entity != nullptr)
	{
		setEntityContainerName(entity->getEntityContainerName());
		setEntityContainerID(entity->getEntityContainerID());
		setValueName(entity->getValueName());
		setValueID(entity->getValueID());
		setFilter(entity->getFilter());
		setIncludeRoot(entity->getIncludeRoot());
		setRecursive(entity->getRecursive());

		return;
	}

	EntityPropertiesSelection* selection = dynamic_cast<EntityPropertiesSelection*>(other);
	assert(selection != nullptr);
	assert(root != nullptr);

	if (selection != nullptr)
	{
		// We keep our current settings for the entity container
		m_valueName = selection->getValue();

		// Now we need to search for the entity ID for the selected name
		EntityBase* entity = findEntityFromName(root, m_valueName);
		assert(entity != nullptr);

		if (entity != nullptr)
		{
			setValueID(entity->getEntityID());
		}

		// Now we update the container ID
		EntityContainer* container = findContainerFromID(root, getEntityContainerID());
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

EntityPropertiesEntityList* EntityPropertiesEntityList::createProperty(const std::string& group, const std::string& name, const std::string& contName, ot::UID contID, const std::string& valName, ot::UID valID, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, valName);

	// Finally create the new property
	EntityPropertiesEntityList* newProperty = new EntityPropertiesEntityList(name, contName, contID, value, valID);
	properties.createProperty(newProperty, group);
	return newProperty;
}

void EntityPropertiesEntityList::updateValueAndContainer(EntityBase* _root, std::list<std::string>& _containerOptions)
{
	OTAssertNullptr(_root);

	EntityContainer* container = this->findContainerFromID(_root, getEntityContainerID());
	if (container)
	{
		this->setEntityContainerName(container->getName());
	}
	else
	{
		container = this->findContainerFromName(_root, getEntityContainerName());
		if (container)
		{
			this->setEntityContainerID(container->getEntityID());
		}
	}

	if (container)
	{
		if (getIncludeRoot())
		{
			_containerOptions.push_back(container->getName());
		}

		for (auto child : container->getChildrenList())
		{
			appendItemToList(child, getFilter(), getRecursive(), _containerOptions);
		}
	}
	else
	{
		//OT_LOG_E("Container not found"); For copied items, e.g. mesh data items, this information is now available, so the current information in the entity should be used.
		//							        No error message should be shown in this case.
	}

	EntityBase* entity = this->findEntityFromID(_root, this->getValueID());

	if (entity != nullptr)
	{
		this->setValueName(entity->getName());
	}
	else
	{
		entity = this->findEntityFromName(_root, this->getValueName());
		if (entity != nullptr)
		{
			this->setValueID(entity->getEntityID());
		}
		else
		{
			//OT_LOG_E("Value not found");  For copied items, e.g. mesh data items, this information is now available, so the current information in the entity should be used.
			//							     No error message should be shown in this case.
		}
	}
}

void EntityPropertiesEntityList::appendItemToList(EntityBase* item, const std::string& filter, bool recursive, std::list<std::string>& _containerOptions)
{
	if (filter.empty())
	{
		_containerOptions.push_back(item->getName());
	}
	else
	{
		if (item->getClassName() == getFilter())
		{
			_containerOptions.push_back(item->getName());
		}
	}

	// Process the children, if recursive search
	if (recursive)
	{
		EntityContainer* container = dynamic_cast<EntityContainer*>(item);
		if (container != nullptr)
		{
			for (auto child : container->getChildrenList())
			{
				appendItemToList(child, filter, recursive, _containerOptions);
			}
		}
	}
}

EntityContainer* EntityPropertiesEntityList::findContainerFromID(EntityBase* root, ot::UID entityID)
{
	EntityContainer* container = dynamic_cast<EntityContainer*>(root);

	if (container != nullptr)
	{
		if (container->getEntityID() == entityID) return container;

		for (auto child : container->getChildrenList())
		{
			EntityContainer* containerChild = findContainerFromID(child, entityID);
			if (containerChild != nullptr) return containerChild;
		}
	}

	return nullptr;
}

EntityContainer* EntityPropertiesEntityList::findContainerFromName(EntityBase* root, const std::string& entityName)
{
	EntityContainer* container = dynamic_cast<EntityContainer*>(root);

	if (container != nullptr)
	{
		if (container->getName() == entityName) return container;

		for (auto child : container->getChildrenList())
		{
			EntityContainer* containerChild = findContainerFromName(child, entityName);
			if (containerChild != nullptr) return containerChild;
		}
	}

	return nullptr;
}

EntityBase* EntityPropertiesEntityList::findEntityFromName(EntityBase* root, const std::string& entityName)
{
	if (root->getName() == entityName) return root;

	EntityContainer* container = dynamic_cast<EntityContainer*>(root);

	if (container != nullptr)
	{
		for (auto child : container->getChildrenList())
		{
			EntityBase* containerChild = findEntityFromName(child, entityName);
			if (containerChild != nullptr) return containerChild;
		}
	}

	return nullptr;
}

EntityBase* EntityPropertiesEntityList::findEntityFromID(EntityBase* root, ot::UID entityID)
{
	if (root->getEntityID() == entityID) return root;

	EntityContainer* container = dynamic_cast<EntityContainer*>(root);

	if (container != nullptr)
	{
		for (auto child : container->getChildrenList())
		{
			EntityBase* containerChild = findEntityFromID(child, entityID);
			if (containerChild != nullptr) return containerChild;
		}
	}

	return nullptr;
}
