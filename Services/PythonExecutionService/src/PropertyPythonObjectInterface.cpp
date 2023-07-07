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

	if (_propertyDouble != nullptr)
	{
		const double value = _propertyDouble->getValue();
		returnValue.reset(PythonObjectBuilder::INSTANCE()->setDouble(value));
	}
	else if (_propertyInt32 != nullptr)
	{
		const int32_t value = _propertyInt32->getValue();
		returnValue.reset(PythonObjectBuilder::INSTANCE()->setInt32(value));
	}
	else if (_propertyBool != nullptr)
	{
		const bool value = _propertyBool->getValue();
		returnValue.reset(PythonObjectBuilder::INSTANCE()->setBool(value));
	}
	else if (_propertyString != nullptr)
	{
		const std::string value = _propertyString->getValue();
		returnValue.reset(PythonObjectBuilder::INSTANCE()->setString(value));
	}
	else if (_propertySelection != nullptr)
	{
		const std::string value = _propertySelection->getValue();
		returnValue.reset(PythonObjectBuilder::INSTANCE()->setString(value));
	}
	else if (_propertyEntityList != nullptr)
	{
		const std::string value = _propertyEntityList->getValueName();
		returnValue.reset(PythonObjectBuilder::INSTANCE()->setString(value));
	}

	return returnValue;
}

void PropertyPythonObjectInterface::SetValue(CPythonObject& cpythonObject)
{
	if (_propertyDouble != nullptr)
	{
		const double value = PythonObjectBuilder::INSTANCE()->getDoubleValue(cpythonObject,"Parameter");
		_propertyDouble->setValue(value);
	}
	else if (_propertyInt32 != nullptr)
	{
		const int32_t value = PythonObjectBuilder::INSTANCE()->getInt32Value(cpythonObject, "Parameter");
		_propertyInt32->setValue(value);
	}
	else if (_propertyBool != nullptr)
	{
		const bool value = PythonObjectBuilder::INSTANCE()->getBoolValue(cpythonObject, "Parameter");
		_propertyBool->setValue(value);
	}
	else if (_propertyString != nullptr)
	{
		const std::string value = PythonObjectBuilder::INSTANCE()->getStringValue(cpythonObject,"Parameter");
		_propertyString->setValue(value);
	}
	else if (_propertySelection != nullptr)
	{
		const std::string value = PythonObjectBuilder::INSTANCE()->getStringValue(cpythonObject, "Parameter");
		_propertySelection->setValue(value);
	}
	else if (_propertyEntityList != nullptr)
	{
		throw std::exception("Entitylist are not supported by the Python Interface, yet.");
		const std::string value = PythonObjectBuilder::INSTANCE()->getStringValue(cpythonObject, "Parameter");
		_propertyEntityList->setValueName(value);
		//Needs the ID and Version set as well
	}
}

