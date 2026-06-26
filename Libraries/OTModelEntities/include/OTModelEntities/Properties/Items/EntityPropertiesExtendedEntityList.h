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
	virtual ~EntityPropertiesExtendedEntityList() {};

	EntityPropertiesExtendedEntityList(const std::string& n, const std::string& contName, ot::UID contID, const std::string& valName, ot::UID valID, const std::vector<std::string>& prefixOptions = std::vector<std::string>(), const std::vector<std::string>& suffixOptions = std::vector<std::string>());

	EntityPropertiesExtendedEntityList(const EntityPropertiesExtendedEntityList& other);

	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesExtendedEntityList(*this); };

	EntityPropertiesExtendedEntityList& operator=(const EntityPropertiesExtendedEntityList& other);

	virtual eType getType() const override { return EXTENDEDENTITYLIST; };
	virtual std::string getTypeString() const override { return EntityPropertiesExtendedEntityList::typeString(); };

	//! \brief Prefix options management.
	//! Prefix options are displayed at the beginning of the dropdown list.
	//! They do NOT correspond to actual entities in the container.
	void clearPrefixOptions();
	void addPrefixOption(const std::string& option);
	void setPrefixOptions(const std::vector<std::string>& options);
	const std::vector<std::string>& getPrefixOptions() const;

	//! \brief Suffix options management.
	//! Suffix options are displayed at the end of the dropdown list.
	//! They do NOT correspond to actual entities in the container.
	void clearSuffixOptions();
	void addSuffixOption(const std::string& option);
	void setSuffixOptions(const std::vector<std::string>& options);
	const std::vector<std::string>& getSuffixOptions() const;

	virtual bool isCompatible(EntityPropertiesBase* other) const override;
	virtual bool hasSameValue(EntityPropertiesBase* other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root) override;

	static EntityPropertiesExtendedEntityList* createProperty(const std::string& group, const std::string& name,
		const std::string& contName, ot::UID contID, const std::string& valName, ot::UID valID,
		const std::vector<std::string>& prefixOptions, const std::vector<std::string>& suffixOptions, const std::string& defaultCategory, EntityProperties& properties);

private:
	std::vector<std::string> m_prefixOptions;
	std::vector<std::string> m_suffixOptions;
};
