// @otlicense

#include "PropertyHelper.h"
#include "OTCore/LogDispatcher.h"

// ###########################################################################################################################################################################################################################################################################################################################

// General

bool PropertyHelper::hasProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	auto allProperties = _base->getProperties().getListOfAllProperties();	
	for (auto property : allProperties)
	{
		if (property->getName() == _name)
		{
			if (_groupName == "")
			{
				return true;
			}
			else if (_groupName == property->getGroup())
			{
				return true;
			}
		}
	}
	return false;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Value getter

double PropertyHelper::getDoublePropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesDouble* doubleProperty = getDoubleProperty(_base, _name, _groupName);
	return doubleProperty->getValue();
}

std::string PropertyHelper::getStringPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesString* stringProperty = getStringProperty(_base, _name, _groupName);
	return stringProperty->getValue();
}

std::string PropertyHelper::getSelectionPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesSelection* selectionProperty = getSelectionProperty(_base, _name, _groupName);
	return selectionProperty->getValue();
}

bool PropertyHelper::getBoolPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesBoolean* boolProperty = getBoolProperty(_base, _name, _groupName);
	return boolProperty->getValue();
}

ot::Color PropertyHelper::getColourPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesColor* colourProperty = getColourProperty(_base, _name, _groupName);

	const int gridColorR = static_cast<int>(colourProperty->getColorR() * 255.0 + 0.5);
	const int gridColorG = static_cast<int>(colourProperty->getColorG() * 255.0 + 0.5);
	const int gridColorB = static_cast<int>(colourProperty->getColorB() * 255.0 + 0.5);
	const ot::Color colour(gridColorR, gridColorG, gridColorB);

	return colour;
}

const ot::Painter2D* PropertyHelper::getPainterPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesGuiPainter* painterProperty = getPainterProperty(_base, _name, _groupName);
	return painterProperty->getValue();
}

int32_t PropertyHelper::getIntegerPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesInteger* intProperty = getIntegerProperty(_base, _name, _groupName);
	return intProperty->getValue();
}

std::string PropertyHelper::getProjectPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesProjectList* projectProperty = getEntityProjectListProperty(_base, _name, _groupName);
	const std::string selectedProjectName = projectProperty->getValue();
	return selectedProjectName;
}

const EntityPropertiesProjectList* PropertyHelper::getEntityProjectListProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr)
	{
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	const EntityPropertiesProjectList* entitySelection = dynamic_cast<const EntityPropertiesProjectList*>(propertyBase);
	if (entitySelection == nullptr)
	{
		throw std::exception(("Tried to cast property " + _name + " to wrong type: entity project selection").c_str());
	}

	return entitySelection;
}



// ###########################################################################################################################################################################################################################################################################################################################

// Value setter

void PropertyHelper::setDoublePropertyValue(double _value, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesDouble* doubleProperty = getDoubleProperty(_base, _name, _groupName);
	doubleProperty->setValue(_value);
}

void PropertyHelper::setStringPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesString* stringProperty = getStringProperty(_base, _name, _groupName);
	stringProperty->setValue(_value);
}

void PropertyHelper::setSelectionPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesSelection* selectionProperty = getSelectionProperty(_base, _name, _groupName);
	const std::vector<std::string> options = selectionProperty->getOptions();
	bool valueIsNoOption = std::find(options.begin(), options.end(), _value) == options.end();
	if (valueIsNoOption) {
		throw std::exception("Tried to set a value that is not an option in the selection entity.");
	}

	selectionProperty->setValue(_value);
}

void PropertyHelper::setSelectionPropertyValue(const std::list<std::string>& _values, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesSelection* selectionProperty = getSelectionProperty(_base, _name, _groupName);
	selectionProperty->resetOptions(_values);
}

void PropertyHelper::setBoolPropertyValue(bool _value, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesBoolean* booleanProeprty = getBoolProperty(_base, _name, _groupName);
	booleanProeprty->setValue(_value);
}

void PropertyHelper::setColourPropertyValue(ot::Color _value, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesColor* colourProperty = getColourProperty(_base, _name, _groupName);

	const double gridColorR = (static_cast<double>(_value.r()) - 0.5) / 255.0;
	const double gridColorG = (static_cast<double>(_value.g()) - 0.5) / 255.0;
	const double gridColorB = (static_cast<double>(_value.b()) - 0.5) / 255.0;
	const double gridColorA = (static_cast<double>(_value.a()) - 0.5) / 255.0;

	colourProperty->setColorRGB(gridColorR, gridColorG, gridColorB);
}

void PropertyHelper::setPainterPropertyValue(const ot::Painter2D* _painter, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesGuiPainter* painterProperty = getPainterProperty(_base, _name, _groupName);
	painterProperty->setValue(_painter);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Writable property access

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

EntityPropertiesInteger* PropertyHelper::getIntegerProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr)
	{
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	EntityPropertiesInteger* intProperty = dynamic_cast<EntityPropertiesInteger*>(propertyBase);
	if (intProperty == nullptr)
	{
		throw std::exception(("Tried to cast property " + _name + " to wrong type: integer").c_str());
	}
	return intProperty;
}

EntityPropertiesGuiPainter* PropertyHelper::getPainterProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr)
	{
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	EntityPropertiesGuiPainter* painter = dynamic_cast<EntityPropertiesGuiPainter*>(propertyBase);
	if (painter== nullptr)
	{
		throw std::exception(("Tried to cast property " + _name + " to wrong type: integer").c_str());
	}
	return painter;
}

EntityPropertiesEntityList* PropertyHelper::getEntityListProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr)
	{
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	EntityPropertiesEntityList* entitySelection = dynamic_cast<EntityPropertiesEntityList*>(propertyBase);
	if (entitySelection == nullptr)
	{
		throw std::exception(("Tried to cast property " + _name + " to wrong type: entity selection").c_str());
	}

	return entitySelection;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Read-only property access

const EntityPropertiesDouble* PropertyHelper::getDoubleProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	const EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr) {
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	const EntityPropertiesDouble* doubleProperty = dynamic_cast<const EntityPropertiesDouble*>(propertyBase);
	if (doubleProperty == nullptr) {
		throw std::exception(("Tried to cast property " + _name + " to wrong type: double").c_str());
	}
	return doubleProperty;
}

const EntityPropertiesString* PropertyHelper::getStringProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	const EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr) {
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	const EntityPropertiesString* stringProperty = dynamic_cast<const EntityPropertiesString*>(propertyBase);
	if (stringProperty == nullptr) {
		throw std::exception(("Tried to cast property " + _name + " to wrong type: string").c_str());
	}
	return stringProperty;
}

const EntityPropertiesSelection* PropertyHelper::getSelectionProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	const EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr) {
		throw std::exception(("Failed to access property " + _name).c_str());
	}
	const EntityPropertiesSelection* selectionProperty = dynamic_cast<const EntityPropertiesSelection*>(propertyBase);
	if (selectionProperty == nullptr) {
		throw std::exception(("Tried to cast property " + _name + " to wrong type: string").c_str());
	}
	return selectionProperty;
}

const EntityPropertiesBoolean* PropertyHelper::getBoolProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	const EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr) {
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	const EntityPropertiesBoolean* boolProperty = dynamic_cast<const EntityPropertiesBoolean*>(propertyBase);
	if (boolProperty == nullptr) {
		throw std::exception(("Tried to cast property " + _name + " to wrong type: bool").c_str());
	}
	return boolProperty;
}

const EntityPropertiesColor* PropertyHelper::getColourProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	const EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr) {
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	const EntityPropertiesColor* colourProperty = dynamic_cast<const EntityPropertiesColor*>(propertyBase);
	if (colourProperty == nullptr) {
		throw std::exception(("Tried to cast property " + _name + " to wrong type: bool").c_str());
	}
	return colourProperty;
}

const EntityPropertiesInteger* PropertyHelper::getIntegerProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	const EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr) {
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	const EntityPropertiesInteger* intProperty = dynamic_cast<const EntityPropertiesInteger*>(propertyBase);
	if (intProperty == nullptr) {
		throw std::exception(("Tried to cast property " + _name + " to wrong type: integer").c_str());
	}
	return intProperty;
}

const EntityPropertiesGuiPainter* PropertyHelper::getPainterProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	const EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr) {
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	const EntityPropertiesGuiPainter* painter = dynamic_cast<const EntityPropertiesGuiPainter*>(propertyBase);
	if (painter == nullptr) {
		throw std::exception(("Tried to cast property " + _name + " to wrong type: integer").c_str());
	}
	return painter;
}

const EntityPropertiesEntityList* PropertyHelper::getEntityListProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	const EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr) {
		throw std::exception(("Failed to access property " + _name).c_str());
	}

	const EntityPropertiesEntityList* entitySelection = dynamic_cast<const EntityPropertiesEntityList*>(propertyBase);
	if (entitySelection == nullptr) {
		throw std::exception(("Tried to cast property " + _name + " to wrong type: entity selection").c_str());
	}

	return entitySelection;
}
