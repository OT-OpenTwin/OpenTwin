// @otlicense
// File: PropertyPythonObjectConverter.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "PropertyPythonObjectConverter.h"
#include "PythonObjectBuilder.h"

PropertyPythonObjectConverter::PropertyPythonObjectConverter(EntityPropertiesBase* property)
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

	throw std::exception("Not supported entity type");
}

PyObject* PropertyPythonObjectConverter::GetValue()
{
	PyObject* returnValue(nullptr);
	PythonObjectBuilder pyObBuilder;
	if (_propertyDouble != nullptr)
	{
		const double value = _propertyDouble->getValue();
		returnValue = PyFloat_FromDouble(value);
	}
	else if (_propertyInt32 != nullptr)
	{
		const int32_t value = _propertyInt32->getValue();
		returnValue = PyLong_FromLong(value);
	}
	else if (_propertyBool != nullptr)
	{
		const bool value = _propertyBool->getValue();
		returnValue = PyBool_FromLong(value);
	}
	else if (_propertyString != nullptr)
	{
		const std::string value = _propertyString->getValue();
		returnValue = PyUnicode_FromString(value.c_str());
	}
	else if (_propertySelection != nullptr)
	{
		const std::string value = _propertySelection->getValue();
		returnValue = PyUnicode_FromString(value.c_str());
	}
	else if (_propertyEntityList != nullptr)
	{
		const std::string value = _propertyEntityList->getValueName();
		returnValue = PyUnicode_FromString(value.c_str());
	}
	else
	{
		assert(0);
	}
	return returnValue;
}

void PropertyPythonObjectConverter::SetValue(const CPythonObject& cpythonObject)
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

