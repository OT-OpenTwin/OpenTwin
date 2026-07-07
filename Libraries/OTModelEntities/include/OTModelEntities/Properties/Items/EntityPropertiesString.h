// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/Properties/Items/EntityPropertiesBase.h"

class OT_MODELENTITIES_API_EXPORT EntityPropertiesString : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "string"; };

	static EntityPropertiesString* createProperty(const std::string& group, const std::string& name, const std::string& defaultValue, const std::string& defaultCategory, EntityProperties& properties);

	EntityPropertiesString();
	virtual ~EntityPropertiesString() = default;

	EntityPropertiesString(const std::string& n, const std::string& v);

	EntityPropertiesString(const EntityPropertiesString& other);

	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesString(*this); };

	EntityPropertiesString& operator=(const EntityPropertiesString& other) { if (&other != this) { EntityPropertiesBase::operator=(other); m_value = other.getValue(); }; return *this; };

	virtual eType getType() const override { return STRING; };
	virtual std::string getTypeString() const override { return EntityPropertiesString::typeString(); };

	void setValue(const std::string& _value);
	const std::string& getValue() const { return m_value; };

	void setIsMultiline(bool _flag) { if (m_isMultiline != _flag) { m_isMultiline = _flag; setNeedsUpdate(); } };
	bool getIsMultiline() const { return m_isMultiline; };

	void setPlaceholderText(const std::string& _placeholderText) { if (m_placeholderText != _placeholderText) { m_placeholderText = _placeholderText; setNeedsUpdate(); } };
	const std::string& getPlaceholderText() const { return m_placeholderText; };

	virtual bool hasSameValue(EntityPropertiesBase* other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

private:
	std::string m_value;
	bool m_isMultiline;
	std::string m_placeholderText;
};
