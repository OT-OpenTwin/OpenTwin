// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Properties/PropertyStringList.h"
#include "OTModelEntities/EntityContainer.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesExtendedEntityList.h"

EntityPropertiesExtendedEntityList::EntityPropertiesExtendedEntityList() : EntityPropertiesEntityList() {}

EntityPropertiesExtendedEntityList::EntityPropertiesExtendedEntityList(const std::string& n, const std::string& contName, ot::UID contID, const std::string& valName, ot::UID valID, const std::vector<std::string>& prefixOptions, const std::vector<std::string>& suffixOptions)
	: EntityPropertiesEntityList(n, contName, contID, valName, valID), m_prefixOptions(prefixOptions), m_suffixOptions(suffixOptions)
{}

EntityPropertiesExtendedEntityList::EntityPropertiesExtendedEntityList(const EntityPropertiesExtendedEntityList& _other)
	: EntityPropertiesEntityList(_other)
{
	m_prefixOptions = _other.m_prefixOptions;
	m_suffixOptions = _other.m_suffixOptions;
}

EntityPropertiesExtendedEntityList& EntityPropertiesExtendedEntityList::operator=(const EntityPropertiesExtendedEntityList& _other)
{
	if (&_other != this)
	{
		EntityPropertiesEntityList::operator=(_other);
		m_prefixOptions = _other.m_prefixOptions;
		m_suffixOptions = _other.m_suffixOptions;
	}
	return *this;
}

void EntityPropertiesExtendedEntityList::clearPrefixOptions()
{
	m_prefixOptions.clear();
}

void EntityPropertiesExtendedEntityList::addPrefixOption(const std::string& option)
{
	if (std::find(m_prefixOptions.begin(), m_prefixOptions.end(), option) == m_prefixOptions.end())
	{
		m_prefixOptions.push_back(option);
	}
}

void EntityPropertiesExtendedEntityList::setPrefixOptions(const std::vector<std::string>& options)
{
	m_prefixOptions = options;
}

const std::vector<std::string>& EntityPropertiesExtendedEntityList::getPrefixOptions() const
{
	return m_prefixOptions;
}

void EntityPropertiesExtendedEntityList::clearSuffixOptions()
{
	m_suffixOptions.clear();
}

void EntityPropertiesExtendedEntityList::addSuffixOption(const std::string& option)
{
	if (std::find(m_suffixOptions.begin(), m_suffixOptions.end(), option) == m_suffixOptions.end())
	{
		m_suffixOptions.push_back(option);
	}
}

void EntityPropertiesExtendedEntityList::setSuffixOptions(const std::vector<std::string>& options)
{
	m_suffixOptions = options;
}

const std::vector<std::string>& EntityPropertiesExtendedEntityList::getSuffixOptions() const
{
	return m_suffixOptions;
}

bool EntityPropertiesExtendedEntityList::isCompatible(EntityPropertiesBase* other) const
{

	if (!EntityPropertiesEntityList::isCompatible(other)) return false;

	EntityPropertiesExtendedEntityList* otherList = dynamic_cast<EntityPropertiesExtendedEntityList*>(other);
	if (otherList == nullptr)
	{
		return true;
	}

	if (m_prefixOptions.size() != otherList->m_prefixOptions.size()) return false;
	for (size_t i = 0; i < m_prefixOptions.size(); i++)
	{
		if (m_prefixOptions[i] != otherList->m_prefixOptions[i]) return false;
	}

	if (m_suffixOptions.size() != otherList->m_suffixOptions.size()) return false;
	for (size_t i = 0; i < m_suffixOptions.size(); i++)
	{
		if (m_suffixOptions[i] != otherList->m_suffixOptions[i]) return false;
	}

	return true;
}

void EntityPropertiesExtendedEntityList::setFromConfiguration(const ot::Property* _property, EntityBase* root)
{

	// 1. Load prefix/suffix options from JSON
	// 2. Load entity data (Container/Value Name and ID)
	// 3. Set the new ValueName from UI input
	// 4. Check if the new value is a prefix/suffix option
	//    - If YES: Set ValueID to invalidUID (no entity exists)
	//    - If NO: Call updateValueAndContainer() to sync name/ID


	std::string oldValueName = getValueName();

	EntityPropertiesBase::setFromConfiguration(_property, root);

	const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList*>(_property);
	if (!actualProperty)
	{
		OT_LOG_E("Property cast failed");
		return;
	}

	//Prefix options
	std::string extendedData = actualProperty->getAdditionalPropertyData("ExtendedOptions");
	if (!extendedData.empty())
	{
		ot::JsonDocument dataDoc;
		dataDoc.fromJson(extendedData);
		// Read prefix options
		if (dataDoc.HasMember("PrefixOptions") && dataDoc["PrefixOptions"].IsArray())
		{
			m_prefixOptions.clear();
			const auto& prefixArray = dataDoc["PrefixOptions"].GetArray();
			for (const auto& opt : prefixArray)
			{
				if (opt.IsString())
				{
					m_prefixOptions.push_back(opt.GetString());
				}
			}
		}
		// Read suffix options
		if (dataDoc.HasMember("SuffixOptions") && dataDoc["SuffixOptions"].IsArray())
		{
			m_suffixOptions.clear();
			const auto& suffixArray = dataDoc["SuffixOptions"].GetArray();
			for (const auto& opt : suffixArray)
			{
				if (opt.IsString())
				{
					m_suffixOptions.push_back(opt.GetString());
				}
			}
		}
	}

	std::string entityData = actualProperty->getAdditionalPropertyData("EntityData");
	if (!entityData.empty())
	{
		ot::JsonDocument dataDoc;
		dataDoc.fromJson(entityData);

		this->setEntityContainerName(ot::json::getString(dataDoc, "ContainerName"));
		this->setEntityContainerID(ot::json::getUInt64(dataDoc, "ContainerID"));
		this->setValueID(ot::invalidUID);
	}


	this->setValueName(actualProperty->getCurrent());


	bool isPrefixOrSuffix = false;
	for (const auto& prefixOption : m_prefixOptions)
	{
		if (getValueName() == prefixOption)
		{
			isPrefixOrSuffix = true;
			break;
		}
	}

	for (const auto& suffixOption : m_suffixOptions)
	{
		if (getValueName() == suffixOption)
		{
			isPrefixOrSuffix = true;
			break;
		}
	}

	if (isPrefixOrSuffix)
	{
		this->setValueID(ot::invalidUID);
	}

	//Update valueID and containerID
	if (root && !isPrefixOrSuffix)
	{
		std::list<std::string> opt;
		this->updateValueAndContainer(root, opt);
	}


}

void EntityPropertiesExtendedEntityList::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	// Creates a dropdown list with the following structure:
	// 1. All prefix options
	// 2. All direct children of the container entity
	// 3. All suffix options 
	// Note: Does NOT call updateValueAndContainer() to avoid overwriting the current ValueName if it's a prefix/suffix option.

	std::list<std::string> opt;

	for (const auto& prefixOption : m_prefixOptions)
	{
		opt.push_back(prefixOption);
	}

	if (root != nullptr)
	{
		EntityContainer* container = this->findContainerFromID(root, getEntityContainerID());
		if (!container)
		{
			container = this->findContainerFromName(root, getEntityContainerName());
		}

		if (container)
		{
			for (auto child : container->getChildrenList())
			{
				opt.push_back(child->getName());
			}
		}
	}

	for (const auto& suffixOption : m_suffixOptions)
	{
		opt.push_back(suffixOption);
	}

	std::string currentValue = this->getValueName();

	if (!root)
	{
		opt.clear();
	}

	ot::JsonDocument dataDoc;
	dataDoc.AddMember("ContainerName", ot::JsonString(this->getEntityContainerName(), dataDoc.GetAllocator()), dataDoc.GetAllocator());
	dataDoc.AddMember("ContainerID", this->getEntityContainerID(), dataDoc.GetAllocator());
	dataDoc.AddMember("ValueID", this->getValueID(), dataDoc.GetAllocator());

	ot::JsonDocument extendedDataDoc;
	if (!m_prefixOptions.empty())
	{
		extendedDataDoc.AddMember("PrefixOptions", ot::JsonArray(m_prefixOptions, extendedDataDoc.GetAllocator()), extendedDataDoc.GetAllocator());
	}
	if (!m_suffixOptions.empty())
	{
		extendedDataDoc.AddMember("SuffixOptions", ot::JsonArray(m_suffixOptions, extendedDataDoc.GetAllocator()), extendedDataDoc.GetAllocator());
	}

	ot::PropertyStringList* newProp = new ot::PropertyStringList(this->getName(), this->getValueName(), opt);
	newProp->setSpecialType("ExtendedEntityList");
	newProp->addAdditionalPropertyData("EntityData", dataDoc.toJson());

	if (!extendedDataDoc.IsNull())
	{
		newProp->addAdditionalPropertyData("ExtendedOptions", extendedDataDoc.toJson());
	}

	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesExtendedEntityList::readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root)
{

	EntityPropertiesEntityList::readFromJsonObject(object, root);

	// Read prefix options
	if (object.HasMember("PrefixOptions") && object["PrefixOptions"].IsArray())
	{
		m_prefixOptions.clear();
		const auto& prefixArray = object["PrefixOptions"].GetArray();
		for (const auto& opt : prefixArray)
		{
			if (opt.IsString())
			{
				m_prefixOptions.push_back(opt.GetString());
			}
		}
	}

	// Read suffix options
	if (object.HasMember("SuffixOptions") && object["SuffixOptions"].IsArray())
	{
		m_suffixOptions.clear();
		const auto& suffixArray = object["SuffixOptions"].GetArray();
		for (const auto& opt : suffixArray)
		{
			if (opt.IsString())
			{
				m_suffixOptions.push_back(opt.GetString());
			}
		}
	}
}

void EntityPropertiesExtendedEntityList::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesEntityList::addToJsonObject(_jsonObject, _allocator, _root);
	if (!m_prefixOptions.empty())
	{
		_jsonObject.AddMember("PrefixOptions", ot::JsonArray(m_prefixOptions, _allocator), _allocator);
	}

	if (!m_suffixOptions.empty())
	{
		_jsonObject.AddMember("SuffixOptions", ot::JsonArray(m_suffixOptions, _allocator), _allocator);
	}
}

void EntityPropertiesExtendedEntityList::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesEntityList::copySettings(other, root);

	EntityPropertiesExtendedEntityList* otherList = dynamic_cast<EntityPropertiesExtendedEntityList*>(other);
	if (otherList != nullptr)
	{
		m_prefixOptions = otherList->m_prefixOptions;
		m_suffixOptions = otherList->m_suffixOptions;
	}
}

bool EntityPropertiesExtendedEntityList::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesExtendedEntityList* entity = dynamic_cast<EntityPropertiesExtendedEntityList*>(other);

	if (entity == nullptr) return false;

	// Compare prefix options
	if (m_prefixOptions.size() != entity->m_prefixOptions.size()) return false;
	for (size_t i = 0; i < m_prefixOptions.size(); i++)
	{
		if (m_prefixOptions[i] != entity->m_prefixOptions[i]) return false;
	}

	// Compare suffix options
	if (m_suffixOptions.size() != entity->m_suffixOptions.size()) return false;
	for (size_t i = 0; i < m_suffixOptions.size(); i++)
	{
		if (m_suffixOptions[i] != entity->m_suffixOptions[i]) return false;
	}
	return EntityPropertiesEntityList::hasSameValue(other);

}

EntityPropertiesExtendedEntityList* EntityPropertiesExtendedEntityList::createProperty(const std::string& group, const std::string& name, const std::string& contName, ot::UID contID,
	const std::string& valName, ot::UID valID, const std::vector<std::string>& prefixOptions,
	const std::vector<std::string>& suffixOptions, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, valName);

	// Finally create the new property
	EntityPropertiesExtendedEntityList* newProperty = new EntityPropertiesExtendedEntityList(name, contName, contID, value, valID, prefixOptions, suffixOptions);
	properties.createProperty(newProperty, group);
	return newProperty;
}
