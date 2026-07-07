// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/Properties/Items/EntityPropertiesEntityList.h"

// std header
#include <vector>

class OT_MODELENTITIES_API_EXPORT EntityPropertiesExtendedEntityList : public EntityPropertiesEntityList
{
public:
	static std::string typeString() { return "extendedentityList"; };

	EntityPropertiesExtendedEntityList();
	EntityPropertiesExtendedEntityList(const std::string& n, const std::string& contName, ot::UID contID, const std::string& valName, ot::UID valID);
	EntityPropertiesExtendedEntityList(const EntityPropertiesExtendedEntityList& other);
	virtual ~EntityPropertiesExtendedEntityList() {};

	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesExtendedEntityList(*this); };

	EntityPropertiesExtendedEntityList& operator=(const EntityPropertiesExtendedEntityList& other);

	virtual eType getType() const override { return EXTENDEDENTITYLIST; };
	virtual std::string getTypeString() const override { return EntityPropertiesExtendedEntityList::typeString(); };

	void setPrefixOptions(const std::vector<std::pair<std::string, ot::PropertyBase::ValueHandlingType>>& _options) { m_prefixOptions = _options; };
	void addPrefixOption(const std::string& _option, ot::PropertyBase::ValueHandlingType _valueHandlingType = ot::PropertyBase::ValueHandlingType::Value);
	const std::vector<std::pair<std::string, ot::PropertyBase::ValueHandlingType>>& getPrefixOptions() const { return m_prefixOptions; };
	void clearPrefixOptions();

	void setSuffixOptions(const std::vector<std::pair<std::string, ot::PropertyBase::ValueHandlingType>>& _options) { m_suffixOptions = _options; };
	void addSuffixOption(const std::string& _option, ot::PropertyBase::ValueHandlingType _valueHandlingType = ot::PropertyBase::ValueHandlingType::Value);
	const std::vector<std::pair<std::string, ot::PropertyBase::ValueHandlingType>>& getSuffixOptions() const { return m_suffixOptions; };
	void clearSuffixOptions();

	virtual bool isCompatible(EntityPropertiesBase* other) const override;
	virtual bool hasSameValue(EntityPropertiesBase* other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root) override;

	virtual ot::PropertyBase::ValueHandlingType getCurrentValueHandlingType() const override { return m_currentValueHandlingType; };

	static EntityPropertiesExtendedEntityList* createProperty(const std::string& group, const std::string& name,
		const std::string& contName, ot::UID contID, const std::string& valName, ot::UID valID,
		const std::string& defaultCategory, EntityProperties& properties);

private:
	ot::PropertyBase::ValueHandlingType m_currentValueHandlingType;
	std::vector<std::pair<std::string, ot::PropertyBase::ValueHandlingType>> m_prefixOptions;
	std::vector<std::pair<std::string, ot::PropertyBase::ValueHandlingType>> m_suffixOptions;
};
