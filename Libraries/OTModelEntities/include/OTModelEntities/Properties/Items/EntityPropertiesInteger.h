// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/Properties/Items/EntityPropertiesBase.h"

class OT_MODELENTITIES_API_EXPORT EntityPropertiesInteger : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "integer"; };

	EntityPropertiesInteger();
	virtual ~EntityPropertiesInteger() {};

	EntityPropertiesInteger(const std::string& _name, long _value);

	EntityPropertiesInteger(const EntityPropertiesInteger& _other);

	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesInteger(*this); };

	EntityPropertiesInteger& operator=(const EntityPropertiesInteger& _other);

	virtual eType getType() const override { return INTEGER; };
	virtual std::string getTypeString() const override { return EntityPropertiesInteger::typeString(); };

	void setValue(long _value);
	long getValue() const { return m_value; };

	void setRange(long _min, long _max);
	void setMin(long _min);
	long getMin() const { return m_min; };
	void setMax(long _max);
	long getMax() const { return m_max; };

	void setAllowCustomValues(bool _allowCustomValues);
	bool getAllowCustomValues() const { return m_allowCustomValues; };

	void setSuffix(const std::string& _suffix);
	const std::string& getSuffix() const { return m_suffix; };

	virtual bool hasSameValue(EntityPropertiesBase* other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root) override;

	static EntityPropertiesInteger* createProperty(const std::string& _group, const std::string& _name, long _defaultValue, const std::string& _defaultCategory, EntityProperties& _properties);
	static EntityPropertiesInteger* createProperty(const std::string& _group, const std::string& _name, long _defaultValue, long _minValue, long _maxValue, const std::string& _defaultCategory, EntityProperties& _properties);

private:
	long m_value;
	bool m_allowCustomValues;
	long m_min;
	long m_max;
	std::string m_suffix;
};
