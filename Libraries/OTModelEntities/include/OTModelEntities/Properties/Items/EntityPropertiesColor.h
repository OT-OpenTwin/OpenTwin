// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/Properties/Items/EntityPropertiesBase.h"

class OT_MODELENTITIES_API_EXPORT EntityPropertiesColor : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "color"; };

	EntityPropertiesColor() : m_color{ 0.0, 0.0, 0.0 } {};

	EntityPropertiesColor(const EntityPropertiesColor& other) : EntityPropertiesBase(other) { m_color[0] = other.getColorR(); m_color[1] = other.getColorG(); m_color[2] = other.getColorB(); };

	EntityPropertiesColor(const std::string& n, double r, double g, double b) : m_color{ r, g, b } { setName(n); };
	virtual ~EntityPropertiesColor() {};

	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesColor(*this); };

	EntityPropertiesColor& operator=(const EntityPropertiesColor& other);

	virtual eType getType() const override { return COLOR; };
	virtual std::string getTypeString() const override { return EntityPropertiesColor::typeString(); };

	void setColorR(double c) { if (m_color[0] != c) setNeedsUpdate(); m_color[0] = c; };
	void setColorG(double c) { if (m_color[1] != c) setNeedsUpdate(); m_color[1] = c; };
	void setColorB(double c) { if (m_color[2] != c) setNeedsUpdate(); m_color[2] = c; };

	void setColorRGB(double r, double g, double b) { if (m_color[0] != r || m_color[1] != g || m_color[2] != b) setNeedsUpdate(); m_color[0] = r; m_color[1] = g; m_color[2] = b; };

	double getColorR() const { return m_color[0]; };
	double getColorG() const { return m_color[1]; };
	double getColorB() const { return m_color[2]; };

	virtual bool hasSameValue(EntityPropertiesBase* other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

	static EntityPropertiesColor* createProperty(const std::string& group, const std::string& name, std::vector<int> defaultValue, const std::string& defaultCategory, EntityProperties& properties);

private:
	double m_color[3];
};
