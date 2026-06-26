// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/Properties/Items/EntityPropertiesBase.h"

// std header
#include <vector>

class OT_MODELENTITIES_API_EXPORT EntityPropertiesSelection : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "selection"; };

	EntityPropertiesSelection();
	virtual ~EntityPropertiesSelection() {};

	EntityPropertiesSelection(const EntityPropertiesSelection& _other);

	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesSelection(*this); };

	EntityPropertiesSelection& operator=(const EntityPropertiesSelection& other) { if (&other != this) { assert(checkCompatibilityOfSettings(other)); EntityPropertiesBase::operator=(other); m_value = other.getValue(); }; return *this; };

	virtual eType getType() const override { return SELECTION; };
	virtual std::string getTypeString() const override { return EntityPropertiesSelection::typeString(); };

	virtual bool isCompatible(EntityPropertiesBase* other) const override;

	bool setValue(const std::string& s);
	const std::string& getValue()const { return m_value; };

	void setAllowCustomValues(bool _allowCustomValues);
	bool getAllowCustomValues() const { return m_allowCustomValues; };

	void clearOptions() { m_options.clear(); };
	void addOption(const std::string& option) { assert(std::find(m_options.begin(), m_options.end(), option) == m_options.end());  m_options.push_back(option); }
	void resetOptions(const std::list<std::string>& _options);
	void resetOptions(const std::vector<std::string>& _options);
	const std::vector<std::string>& getOptions() const { return m_options; };

	virtual bool hasSameValue(EntityPropertiesBase* other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

	static EntityPropertiesSelection* createProperty(const std::string& group, const std::string& name, const std::list<std::string>& options, const std::string& defaultValue, const std::string& defaultCategory, EntityProperties& properties);
	static EntityPropertiesSelection* createProperty(const std::string& group, const std::string& name, std::list<std::string>&& options, const std::string& defaultValue, const std::string& defaultCategory, EntityProperties& properties);

protected:
	//! @brief Resets the options for this property and checks whether the settings of the property are still compatible with the new options.
	//! Clears the current options and iterates through the provided options adding them to the property.
	//! @tparam TContainer Type of the container providing the options. Needs to be iterable and provide std::string elements.
	//! @param _options Container providing the new options for the property.
	template <typename TContainer>
	void resetOptionsImpl(const TContainer& _options);

private:
	bool checkCompatibilityOfSettings(const EntityPropertiesSelection& other) const;

	std::vector<std::string> m_options;
	std::string m_value;
	bool m_allowCustomValues;
};

#include "OTModelEntities/Properties/Items/EntityPropertiesSelection.hpp"
