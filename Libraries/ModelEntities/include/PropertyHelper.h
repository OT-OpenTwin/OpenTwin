#pragma once
#include "EntityBase.h"
#include "OTCore/Color.h"

namespace PropertyHelper
{
	__declspec(dllexport) bool hasProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	__declspec(dllexport) double getDoublePropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) std::string getStringPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) std::string getSelectionPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) bool getBoolPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) ot::Color getColourPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) const ot::Painter2D* getPainterPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) int32_t getIntegerPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	 
	__declspec(dllexport) EntityPropertiesDouble* getDoubleProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName ="");
	__declspec(dllexport) EntityPropertiesString* getStringProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) EntityPropertiesSelection* getSelectionProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) EntityPropertiesBoolean* getBoolProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) EntityPropertiesColor* getColourProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) EntityPropertiesInteger* getIntegerProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) EntityPropertiesGuiPainter* getPainterProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	__declspec(dllexport) void setDoublePropertyValue(double _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) void setStringPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) void setSelectionPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) void setSelectionPropertyValue(const std::list<std::string>& _values, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) void setBoolPropertyValue(bool _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) void setColourPropertyValue(ot::Color _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	__declspec(dllexport) void setPainterPropertyValue(const ot::Painter2D* _painter, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
};

