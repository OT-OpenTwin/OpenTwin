#include "PropertyPythonObjectInterface.h"
#include "PythonObjectBuilder.h"

PropertyPythonObjectInterface::PropertyPythonObjectInterface(EntityPropertiesBase* property)
{
	auto doubleProp = dynamic_cast<EntityPropertiesDouble*>(property);
	if ( doubleProp != nullptr)
	{
		_propertyDouble = doubleProp;
		return;
	}
	auto intProp = dynamic_cast<EntityPropertiesInteger*>(property);
	if(intProp != nullptr)
	{
		_propertyInt32 = intProp;
		return;
	}
	auto boolProp = dynamic_cast<EntityPropertiesBoolean*>(property);
	if (boolProp != nullptr)
	{
		_propertyBool = boolProp;
		return;
	}
	auto stringProp = dynamic_cast<EntityPropertiesString*>(property);
	if (stringProp != nullptr)
	{
		_propertyString = stringProp;
		return;
	}
	auto selectionProp = dynamic_cast<EntityPropertiesSelection*>(property);
	if (selectionProp != nullptr)
	{
		_propertySelection = selectionProp;
		return;
	}
	auto entListProp = dynamic_cast<EntityPropertiesEntityList*>(property);
	if (entListProp!= nullptr)
	{
		_propertyEntityList = entListProp;
		return;
	}
}

CPythonObjectNew PropertyPythonObjectInterface::GetValue()
{
	CPythonObjectNew returnValue(nullptr);
	PythonObjectBuilder pyObBuilder;
	if (_propertyDouble != nullptr)
	{
		const double value = _propertyDouble->getValue();
		returnValue.reset(pyObBuilder.setDouble(value));
	}
	else if (_propertyInt32 != nullptr)
	{
		const int32_t value = _propertyInt32->getValue();
		returnValue.reset(pyObBuilder.setInt32(value));
	}
	else if (_propertyBool != nullptr)
	{
		const bool value = _propertyBool->getValue();
		returnValue.reset(pyObBuilder.setBool(value));
	}
	else if (_propertyString != nullptr)
	{
		const std::string value = _propertyString->getValue();
		returnValue.reset(pyObBuilder.setString(value));
	}
	else if (_propertySelection != nullptr)
	{
		const std::string value = _propertySelection->getValue();
		returnValue.reset(pyObBuilder.setString(value));
	}
	else if (_propertyEntityList != nullptr)
	{
		const std::string value = _propertyEntityList->getValueName();
		returnValue.reset(pyObBuilder.setString(value));
	}

	return returnValue;
}

void PropertyPythonObjectInterface::SetValue(const CPythonObject& cpythonObject)
{
	PythonObjectBuilder pyObBuilder;
	if (_propertyDouble != nullptr)
	{
		const double value = pyObBuilder.getDoubleValue(cpythonObject,"Parameter");
		_propertyDouble->setValue(value);
	}
	else if (_propertyInt32 != nullptr)
	{
		const int32_t value = pyObBuilder.getInt32Value(cpythonObject, "Parameter");
		_propertyInt32->setValue(value);
	}
	else if (_propertyBool != nullptr)
	{
		const bool value = pyObBuilder.getBoolValue(cpythonObject, "Parameter");
		_propertyBool->setValue(value);
	}
	else if (_propertyString != nullptr)
	{
		const std::string value = pyObBuilder.getStringValue(cpythonObject,"Parameter");
		_propertyString->setValue(value);
	}
	else if (_propertySelection != nullptr)
	{
		const std::string value = pyObBuilder.getStringValue(cpythonObject, "Parameter");
		_propertySelection->setValue(value);
	}
	else if (_propertyEntityList != nullptr)
	{
		throw std::exception("Entitylist are not supported by the Python Interface, yet.");
		const std::string value = pyObBuilder.getStringValue(cpythonObject, "Parameter");
		_propertyEntityList->setValueName(value);
		//Needs the ID and Version set as well
	}
}

