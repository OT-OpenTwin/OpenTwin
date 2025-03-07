#include "OTGui/QueryInformation.h"

void ot::QueryInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("Query", JsonString(m_query,_allocator), _allocator);
	_object.AddMember("Projection", JsonString(m_projection,_allocator), _allocator);
	_object.AddMember("QuantityFieldName", JsonString(m_quantityFieldName,_allocator), _allocator);
	_object.AddMember("ParameterFieldName", JsonString(m_parameterFieldName,_allocator), _allocator);
}

void ot::QueryInformation::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	m_query = ot::json::getString(_object, "Query");
	m_projection = ot::json::getString(_object, "Projection");
	m_quantityFieldName= ot::json::getString(_object, "QuantityFieldName");
	m_parameterFieldName= ot::json::getString(_object, "ParameterFieldName");
}
