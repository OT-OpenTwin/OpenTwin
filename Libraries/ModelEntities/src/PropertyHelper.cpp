#include "PropertyHelper.h"
#include "OTCore/Logger.h"

double PropertyHelper::getDoublePropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesDouble* doubleProperty = getDoubleProperty(_base, _name, _groupName);
	return doubleProperty->getValue();
}

std::string PropertyHelper::getStringPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesString* stringProperty = getStringProperty(_base, _name, _groupName);
	return stringProperty->getValue();
}

std::string PropertyHelper::getSelectionPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesSelection* selectionProperty = getSelectionProperty(_base, _name, _groupName);
	return selectionProperty->getValue();
}

bool PropertyHelper::getBoolPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesBoolean* boolProperty = getBoolProperty(_base, _name, _groupName);
	return boolProperty->getValue();
}

ot::Color PropertyHelper::getColourPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesColor* colourProperty = getColourProperty(_base, _name, _groupName);

	const int gridColorR = static_cast<int>(colourProperty->getColorR() * 255.0 + 0.5);
	const int gridColorG = static_cast<int>(colourProperty->getColorG() * 255.0 + 0.5);
	const int gridColorB = static_cast<int>(colourProperty->getColorB() * 255.0 + 0.5);
	const ot::Color colour(gridColorR, gridColorG, gridColorB);

	return colour;
}

int32_t PropertyHelper::getIntegerPropertyValue(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name,_groupName);
	if (propertyBase == nullptr)
	{
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	EntityPropertiesInteger* intProperty= dynamic_cast<EntityPropertiesInteger*>(propertyBase);
	if (intProperty == nullptr)
	{
		throw std::exception(("Tried to cast property " + _name + " to wrong type: double").c_str());
	}
	return intProperty->getValue();
}

EntityPropertiesDouble* PropertyHelper::getDoubleProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr)
	{
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	EntityPropertiesDouble* doubleProperty = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	if (doubleProperty == nullptr)
	{
		throw std::exception(("Tried to cast property " + _name + " to wrong type: double").c_str());
	}
	return doubleProperty;
}

EntityPropertiesString* PropertyHelper::getStringProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr)
	{
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	EntityPropertiesString* stringProperty = dynamic_cast<EntityPropertiesString*>(propertyBase);
	if (stringProperty == nullptr)
	{
		throw std::exception(("Tried to cast property " + _name + " to wrong type: string").c_str());
	}
	return stringProperty;
}

EntityPropertiesSelection* PropertyHelper::getSelectionProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr)
	{
		throw std::exception(("Failed to access property " + _name).c_str());
	}
	EntityPropertiesSelection* selectionProperty = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	if (selectionProperty == nullptr)
	{
		throw std::exception(("Tried to cast property " + _name + " to wrong type: string").c_str());
	}
	return selectionProperty;
}

EntityPropertiesBoolean* PropertyHelper::getBoolProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr)
	{
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	EntityPropertiesBoolean* boolProperty = dynamic_cast<EntityPropertiesBoolean*>(propertyBase);
	if (boolProperty == nullptr)
	{
		throw std::exception(("Tried to cast property " + _name + " to wrong type: bool").c_str());
	}
	return boolProperty;
}

EntityPropertiesColor* PropertyHelper::getColourProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr)
	{
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	EntityPropertiesColor* colourProperty = dynamic_cast<EntityPropertiesColor*>(propertyBase);
	if (colourProperty == nullptr)
	{
		throw std::exception(("Tried to cast property " + _name + " to wrong type: bool").c_str());
	}
	return colourProperty;
}

void PropertyHelper::setDoublePropertyValue(double _value, EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesDouble* doubleProperty = getDoubleProperty(_base, _name, _groupName);
	doubleProperty->setValue(_value);
}

void PropertyHelper::setStringPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesString* stringProperty = getStringProperty(_base, _name, _groupName);
	stringProperty->setValue(_value);
}

void PropertyHelper::setSelectionPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesSelection* selectionProperty = getSelectionProperty(_base, _name, _groupName);
	const std::vector<std::string> options = selectionProperty->getOptions();
	bool valueIsNoOption = std::find(options.begin(), options.end(), _value) == options.end();
	if (valueIsNoOption)
	{
		throw std::exception("Tried to set a value that is not an option in the selection entity.");
	}

	selectionProperty->setValue(_value);
}

void PropertyHelper::setBoolPropertyValue(bool _value, EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesBoolean* booleanProeprty = getBoolProperty(_base, _name, _groupName);
	booleanProeprty->setValue(_value);
}

void PropertyHelper::setColourPropertyValue(ot::Color _value, EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesColor* colourProperty = getColourProperty(_base, _name, _groupName);

	const double gridColorR = (static_cast<double>(_value.r()) - 0.5) / 255.0;
	const double gridColorG = (static_cast<double>(_value.g()) - 0.5) / 255.0;
	const double gridColorB = (static_cast<double>(_value.b()) - 0.5) / 255.0;
	const double gridColorA = (static_cast<double>(_value.a()) - 0.5) / 255.0;
	
	colourProperty->setColorRGB(gridColorR, gridColorG, gridColorB);	

}