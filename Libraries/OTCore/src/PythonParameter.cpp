#include "OTCore/Python/PythonParameter.h"


void PythonParameter::setCallingEntity(const std::string& _entityName)
{
	m_callingEntity = _entityName;
}
void PythonParameter::setAdditionalParameter(ot::JsonDocument&& _parameter)
{
	m_parameter.CopyFrom(_parameter, m_parameter.GetAllocator());
}

void PythonParameter::setPythonParameterType(const std::string& _type)
{
	m_type = _type;
}

void PythonParameter::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const
{
	_jsonObject.AddMember("EntityName", ot::JsonString(m_callingEntity, _allocator), _allocator);
	
	ot::JsonValue param;
	param.CopyFrom(m_parameter, _allocator);
	_jsonObject.AddMember("AdditionalParam", param, _allocator);
	_jsonObject.AddMember("Type", ot::JsonString(m_type, _allocator), _allocator);
}

void PythonParameter::setFromJsonObject(const ot::ConstJsonObject& _jsonObject)
{
	m_callingEntity =	ot::json::getString( _jsonObject, "EntityName");
	m_parameter.CopyFrom(_jsonObject["AdditionalParam"], m_parameter.GetAllocator());
	m_type = ot::json::getString(_jsonObject, "Type");
}


PythonParameter::PythonParameter(const PythonParameter& _other)
	: m_callingEntity(_other.getCallingEntity()), m_type(_other.getPythonParameterType())
{
	m_parameter.CopyFrom(_other.getAdditionalParameter(), m_parameter.GetAllocator());
}

void PythonParameter::swap(PythonParameter& _other)
{
	std::swap(m_callingEntity, _other.m_callingEntity);
	std::swap(m_type, _other.m_type);
	m_parameter.Swap(_other.m_parameter);
}

PythonParameter PythonParameter::operator=(const PythonParameter& _other)
{
	if (this != &_other) 
	{
		PythonParameter tmp(_other);
		swap(tmp);
	}
	return *this;
}

PythonParameter PythonParameter::operator=(PythonParameter&& _other) noexcept
{
	PythonParameter tmp(std::move(_other));
	swap(tmp);          
	return *this;
}

PythonParameter::PythonParameter(PythonParameter&& _other) noexcept
	: PythonParameter()
{
	swap(_other);
}

