// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Properties/PropertyStringList.h"
#include "OTModelEntities/EntityContainer.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesExtendedEntityList.h"

EntityPropertiesExtendedEntityList::EntityPropertiesExtendedEntityList() 
	: EntityPropertiesEntityList(), m_currentValueHandlingType(ot::PropertyBase::ValueHandlingType::Value) {}

EntityPropertiesExtendedEntityList::EntityPropertiesExtendedEntityList(const std::string& n, const std::string& contName, ot::UID contID, const std::string& valName, ot::UID valID)
	: EntityPropertiesEntityList(n, contName, contID, valName, valID), m_currentValueHandlingType(ot::PropertyBase::ValueHandlingType::Value)
{}

EntityPropertiesExtendedEntityList::EntityPropertiesExtendedEntityList(const EntityPropertiesExtendedEntityList& _other)
	: EntityPropertiesEntityList(_other)
{
	m_currentValueHandlingType = _other.m_currentValueHandlingType;
	m_prefixOptions = _other.m_prefixOptions;
	m_suffixOptions = _other.m_suffixOptions;
}

EntityPropertiesExtendedEntityList& EntityPropertiesExtendedEntityList::operator=(const EntityPropertiesExtendedEntityList& _other)
{
	if (&_other != this)
	{
		EntityPropertiesEntityList::operator=(_other);
		m_currentValueHandlingType = _other.m_currentValueHandlingType;
		m_prefixOptions = _other.m_prefixOptions;
		m_suffixOptions = _other.m_suffixOptions;
	}
	return *this;
}

void EntityPropertiesExtendedEntityList::clearPrefixOptions()
{
	m_prefixOptions.clear();
}

void EntityPropertiesExtendedEntityList::addPrefixOption(const std::string& _option, ot::PropertyBase::ValueHandlingType _valueHandlingType)
{
	bool found = false;
	for (const auto& opt : m_prefixOptions)
	{
		if (opt.first == _option)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		m_prefixOptions.push_back(std::make_pair(_option, _valueHandlingType));
	}
}

void EntityPropertiesExtendedEntityList::clearSuffixOptions()
{
	m_suffixOptions.clear();
}

void EntityPropertiesExtendedEntityList::addSuffixOption(const std::string& _option, ot::PropertyBase::ValueHandlingType _valueHandlingType)
{
	bool found = false;
	for (const auto& opt : m_suffixOptions)
	{
		if (opt.first == _option)
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		m_suffixOptions.push_back(std::make_pair(_option, _valueHandlingType));
	}
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
			const auto prefixArray = dataDoc["PrefixOptions"].GetArray();
			for (const auto& opt : prefixArray)
			{
				if (opt.IsString())
				{
					addPrefixOption(opt.GetString(), ot::PropertyBase::ValueHandlingType::Value);
				}
				else if (opt.IsObject())
				{
					std::string optionName = ot::json::getString(opt, "V");
					ot::PropertyBase::ValueHandlingType valueHandlingType = ot::PropertyBase::stringToValueHandlingType(ot::json::getString(opt, "T", ot::PropertyBase::toString(ot::PropertyBase::ValueHandlingType::Value)));
					addPrefixOption(optionName, valueHandlingType);
				}
				else
				{
					OT_LOG_E("Invalid prefix option format in ExtendedOptions JSON");
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
					addSuffixOption(opt.GetString(), ot::PropertyBase::ValueHandlingType::Value);
				}
				else if (opt.IsObject())
				{
					std::string optionName = ot::json::getString(opt, "V");
					ot::PropertyBase::ValueHandlingType valueHandlingType = ot::PropertyBase::stringToValueHandlingType(ot::json::getString(opt, "T", ot::PropertyBase::toString(ot::PropertyBase::ValueHandlingType::Value)));
					addSuffixOption(optionName, valueHandlingType);
				}
				else
				{
					OT_LOG_E("Invalid suffix option format in ExtendedOptions JSON");
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
		if (getValueName() == prefixOption.first)
		{
			isPrefixOrSuffix = true;
			break;
		}
	}

	for (const auto& suffixOption : m_suffixOptions)
	{
		if (getValueName() == suffixOption.first)
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

	m_currentValueHandlingType = actualProperty->getCurrentValueHandlingType();
}

void EntityPropertiesExtendedEntityList::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	// Creates a dropdown list with the following structure:
	// 1. All prefix options
	// 2. All direct children of the container entity
	// 3. All suffix options 
	// Note: Does NOT call updateValueAndContainer() to avoid overwriting the current ValueName if it's a prefix/suffix option.

	std::list<std::pair<std::string, ot::PropertyBase::ValueHandlingType>> opt;

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
				opt.push_back(std::make_pair(child->getName(), ot::PropertyBase::ValueHandlingType::Value));
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
		ot::JsonArray prefixArr;
		for (const auto& prefixOption : m_prefixOptions)
		{
			ot::JsonObject prefixObj;
			prefixObj.AddMember("V", ot::JsonString(prefixOption.first, extendedDataDoc.GetAllocator()), extendedDataDoc.GetAllocator());
			prefixObj.AddMember("T", ot::JsonString(ot::PropertyBase::toString(prefixOption.second), extendedDataDoc.GetAllocator()), extendedDataDoc.GetAllocator());
			prefixArr.PushBack(prefixObj, extendedDataDoc.GetAllocator());
		}
		extendedDataDoc.AddMember("PrefixOptions", prefixArr, extendedDataDoc.GetAllocator());
	}
	if (!m_suffixOptions.empty())
	{
		ot::JsonArray suffixArr;
		for (const auto& suffixOption : m_suffixOptions)
		{
			ot::JsonObject suffixObj;
			suffixObj.AddMember("V", ot::JsonString(suffixOption.first, extendedDataDoc.GetAllocator()), extendedDataDoc.GetAllocator());
			suffixObj.AddMember("T", ot::JsonString(ot::PropertyBase::toString(suffixOption.second), extendedDataDoc.GetAllocator()), extendedDataDoc.GetAllocator());
			suffixArr.PushBack(suffixObj, extendedDataDoc.GetAllocator());
		}
		extendedDataDoc.AddMember("SuffixOptions", suffixArr, extendedDataDoc.GetAllocator());
	}

	std::vector<std::pair<std::string, ot::PropertyBase::ValueHandlingType>> optVec(opt.begin(), opt.end());

	ot::PropertyStringList* newProp = new ot::PropertyStringList(this->getName(), this->getValueName(), std::list<std::string>());
	newProp->setOptions(optVec);
	newProp->setSpecialType("ExtendedEntityList");
	newProp->addAdditionalPropertyData("EntityData", dataDoc.toJson());
	newProp->setCurrentValueHandlingType(m_currentValueHandlingType);

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
				addPrefixOption(opt.GetString(), ot::PropertyBase::ValueHandlingType::Value);
			}
			else if (opt.IsObject())
			{
				std::string optionName = ot::json::getString(opt, "V");
				ot::PropertyBase::ValueHandlingType valueHandlingType = ot::PropertyBase::stringToValueHandlingType(ot::json::getString(opt, "T", ot::PropertyBase::toString(ot::PropertyBase::ValueHandlingType::Value)));
				addPrefixOption(optionName, valueHandlingType);
			}
			else
			{
				OT_LOG_E("Invalid prefix option format in JSON");
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
				addSuffixOption(opt.GetString(), ot::PropertyBase::ValueHandlingType::Value);
			}
			else if (opt.IsObject())
			{
				std::string optionName = ot::json::getString(opt, "V");
				ot::PropertyBase::ValueHandlingType valueHandlingType = ot::PropertyBase::stringToValueHandlingType(ot::json::getString(opt, "T", ot::PropertyBase::toString(ot::PropertyBase::ValueHandlingType::Value)));
				addSuffixOption(optionName, valueHandlingType);
			}
			else
			{
				OT_LOG_E("Invalid suffix option format in JSON");
			}
		}
	}
}

void EntityPropertiesExtendedEntityList::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesEntityList::addToJsonObject(_jsonObject, _allocator, _root);
	if (!m_prefixOptions.empty())
	{
		ot::JsonArray prefixArr;
		for (const auto& prefixOption : m_prefixOptions)
		{
			ot::JsonObject prefixObj;
			prefixObj.AddMember("V", ot::JsonString(prefixOption.first, _allocator), _allocator);
			prefixObj.AddMember("T", ot::JsonString(ot::PropertyBase::toString(prefixOption.second), _allocator), _allocator);
			prefixArr.PushBack(prefixObj, _allocator);
		}
		_jsonObject.AddMember("PrefixOptions", prefixArr, _allocator);
	}

	if (!m_suffixOptions.empty())
	{
		ot::JsonArray suffixArr;
		for (const auto& suffixOption : m_suffixOptions)
		{
			ot::JsonObject suffixObj;
			suffixObj.AddMember("V", ot::JsonString(suffixOption.first, _allocator), _allocator);
			suffixObj.AddMember("T", ot::JsonString(ot::PropertyBase::toString(suffixOption.second), _allocator), _allocator);
			suffixArr.PushBack(suffixObj, _allocator);
		}
		_jsonObject.AddMember("SuffixOptions", suffixArr, _allocator);
	}
}

void EntityPropertiesExtendedEntityList::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesEntityList::copySettings(other, root);

	EntityPropertiesExtendedEntityList* otherList = dynamic_cast<EntityPropertiesExtendedEntityList*>(other);
	if (otherList != nullptr)
	{
		m_currentValueHandlingType = otherList->m_currentValueHandlingType;
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
	const std::string& valName, ot::UID valID, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, valName);

	// Finally create the new property
	EntityPropertiesExtendedEntityList* newProperty = new EntityPropertiesExtendedEntityList(name, contName, contID, value, valID);
	properties.createProperty(newProperty, group);
	return newProperty;
}
