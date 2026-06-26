// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/Properties/Items/EntityPropertiesBase.h"

class OT_MODELENTITIES_API_EXPORT EntityPropertiesDouble : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "double"; };

	EntityPropertiesDouble();
	virtual ~EntityPropertiesDouble() {};

	EntityPropertiesDouble(const std::string& _name, double _value);

	EntityPropertiesDouble(const EntityPropertiesDouble& _other);

	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesDouble(*this); };

	EntityPropertiesDouble& operator=(const EntityPropertiesDouble& _other);

	virtual eType getType() const override { return DOUBLE; };
	virtual std::string getTypeString() const override { return EntityPropertiesDouble::typeString(); };

	void setValue(double _value);
	double getValue() const { return m_value; };

	void setRange(double _min, double _max);
	void setMin(double _min);
	double getMin() const { return m_min; };
	void setMax(double _max);
	double getMax() const { return m_max; };

	void setAllowCustomValues(bool _allowCustomValues);
	bool getAllowCustomValues() const { return m_allowCustomValues; };

	void setSuffix(const std::string& _suffix);
	const std::string& getSuffix() const { return m_suffix; };

	//! @brief Set the decimal places when using a spin control for this property.
	//! This is only relevant for the property grid and does not affect the actual value of the property.
	//! @param _precision Number of decimal places to show in the spin control.
	void setDecimalPlaces(int _precision);
	int getDecimalPlaces() const { return m_precision; };

	virtual bool hasSameValue(EntityPropertiesBase* other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root) override;

	static EntityPropertiesDouble* createProperty(const std::string& _group, const std::string& _name, double _defaultValue, const std::string& _defaultCategory, EntityProperties& _properties);
	static EntityPropertiesDouble* createProperty(const std::string& _group, const std::string& _name, double _defaultValue, double _minValue, double _maxValue, const std::string& _defaultCategory, EntityProperties& _properties);

private:
	double m_value;
	bool m_allowCustomValues;
	double m_min;
	double m_max;
	int m_precision;
	std::string m_suffix;
};
