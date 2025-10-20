#pragma once

#include "EntityBase.h"
#include "OTCore/Color.h"
#include "ModelEntitiesAPIExport.h"

class OT_MODELENTITIES_API_EXPORT PropertyHelper
{
	OT_DECL_STATICONLY(PropertyHelper)
public:
	// ###########################################################################################################################################################################################################################################################################################################################

	// General

	static bool hasProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	// ###########################################################################################################################################################################################################################################################################################################################

	// Value getter

	static double getDoublePropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static std::string getStringPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static std::string getSelectionPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static bool getBoolPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static ot::Color getColourPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static const ot::Painter2D* getPainterPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static int32_t getIntegerPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static std::string getProjectPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	// ###########################################################################################################################################################################################################################################################################################################################

	// Value setter

	static void setDoublePropertyValue(double _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static void setStringPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static void setSelectionPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static void setSelectionPropertyValue(const std::list<std::string>& _values, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static void setBoolPropertyValue(bool _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static void setColourPropertyValue(ot::Color _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static void setPainterPropertyValue(const ot::Painter2D* _painter, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	// ###########################################################################################################################################################################################################################################################################################################################

	// Writable property access

	static EntityPropertiesDouble* getDoubleProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName ="");
	static EntityPropertiesString* getStringProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static EntityPropertiesSelection* getSelectionProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static EntityPropertiesBoolean* getBoolProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static EntityPropertiesColor* getColourProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static EntityPropertiesInteger* getIntegerProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static EntityPropertiesGuiPainter* getPainterProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static EntityPropertiesEntityList* getEntityListProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	// ###########################################################################################################################################################################################################################################################################################################################

	// Read-only property access

	static const EntityPropertiesDouble* getDoubleProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static const EntityPropertiesString* getStringProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static const EntityPropertiesSelection* getSelectionProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static const EntityPropertiesBoolean* getBoolProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static const EntityPropertiesColor* getColourProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static const EntityPropertiesInteger* getIntegerProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static const EntityPropertiesGuiPainter* getPainterProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static const EntityPropertiesEntityList* getEntityListProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	static const EntityPropertiesProjectList* getEntityProjectListProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

};

