// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/Properties/Items/EntityPropertiesBase.h"

class OT_MODELENTITIES_API_EXPORT EntityPropertiesBoolean : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "boolean"; };

	EntityPropertiesBoolean() : m_value(false) {};
	virtual ~EntityPropertiesBoolean() {};

	EntityPropertiesBoolean(const std::string& n, bool v) : m_value(v) { setName(n); };

	EntityPropertiesBoolean(const EntityPropertiesBoolean& other) : EntityPropertiesBase(other) { m_value = other.m_value; };

	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesBoolean(*this); };

	EntityPropertiesBoolean& operator=(const EntityPropertiesBoolean& other) { if (&other != this) { EntityPropertiesBase::operator=(other); m_value = other.getValue(); }; return *this; };

	virtual eType getType() const override { return BOOLEAN; };
	virtual std::string getTypeString() const override { return EntityPropertiesBoolean::typeString(); };

	void setValue(bool b) { if (m_value != b) setNeedsUpdate(); m_value = b; };
	bool getValue() const { return m_value; };

	virtual bool hasSameValue(EntityPropertiesBase* other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

	static EntityPropertiesBoolean* createProperty(const std::string& group, const std::string& name, bool defaultValue, const std::string& defaultCategory, EntityProperties& properties);

private:
	bool m_value;
};
