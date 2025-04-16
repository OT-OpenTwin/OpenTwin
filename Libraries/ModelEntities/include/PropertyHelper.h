#pragma once
#include "EntityBase.h"
#include "OTCore/Color.h"

namespace PropertyHelper
{
	double getDoublePropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	std::string getStringPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	std::string getSelectionPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	bool getBoolPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	ot::Color getColourPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	int32_t getIntegerPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	EntityPropertiesDouble* getDoubleProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName ="");
	EntityPropertiesString* getStringProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	EntityPropertiesSelection* getSelectionProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	EntityPropertiesBoolean* getBoolProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	EntityPropertiesColor* getColourProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	void setDoublePropertyValue(double _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	void setStringPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	void setSelectionPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	void setBoolPropertyValue(bool _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	void setColourPropertyValue(ot::Color _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

};

