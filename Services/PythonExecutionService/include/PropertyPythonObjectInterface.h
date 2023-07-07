#pragma once
#include "EntityPropertiesItems.h"
#include "CPythonObjectNew.h"

class PropertyPythonObjectInterface
{
public:
	PropertyPythonObjectInterface(EntityPropertiesBase* property);
	CPythonObjectNew GetValue();
	void SetValue(CPythonObject& cpythonObject);

private:
	EntityPropertiesDouble* _propertyDouble = nullptr;
	EntityPropertiesInteger* _propertyInt32 = nullptr;
	EntityPropertiesBoolean* _propertyBool = nullptr;
	EntityPropertiesString* _propertyString = nullptr;
	EntityPropertiesSelection* _propertySelection = nullptr;
	EntityPropertiesEntityList* _propertyEntityList = nullptr;
};