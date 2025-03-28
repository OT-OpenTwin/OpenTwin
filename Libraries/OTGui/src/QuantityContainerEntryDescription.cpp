#include "OTGui/QuantityContainerEntryDescription.h"

void ot::QuantityContainerEntryDescription::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("FieldName", JsonString(m_fieldName, _allocator), _allocator);
	_object.AddMember("Label", JsonString(m_label, _allocator), _allocator);
	_object.AddMember("Unit", JsonString(m_unit, _allocator), _allocator);
	_object.AddMember("DataType", JsonString(m_dataType, _allocator), _allocator);
}

void ot::QuantityContainerEntryDescription::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	m_fieldName = ot::json::getString(_object, "FieldName");;
	m_label = ot::json::getString(_object, "Label");;
	m_unit = ot::json::getString(_object, "Unit");;
	m_dataType = ot::json::getString(_object, "DataType");;
}



