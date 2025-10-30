// @otlicense

#pragma once

#include "EntityPropertiesItems.h"
#include "CPythonObjectNew.h"

//! @brief Class that converts a property value into a PythonObject and viceversa.
class PropertyPythonObjectConverter
{
public:
	PropertyPythonObjectConverter(EntityPropertiesBase* property);
	PyObject* GetValue();
	void SetValue(const CPythonObject& cpythonObject);

private:
	EntityPropertiesDouble* _propertyDouble = nullptr;
	EntityPropertiesInteger* _propertyInt32 = nullptr;
	EntityPropertiesBoolean* _propertyBool = nullptr;
	EntityPropertiesString* _propertyString = nullptr;
	EntityPropertiesSelection* _propertySelection = nullptr;
	EntityPropertiesEntityList* _propertyEntityList = nullptr;
};