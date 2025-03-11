#include "OTGui/QueryInformation.h"

void ot::QueryInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("Query", JsonString(m_query,_allocator), _allocator);
	_object.AddMember("Projection", JsonString(m_projection,_allocator), _allocator);
	
	ot::JsonArray quantityContainerEntryDescription;
	for (auto& parameterDescription : m_parameterDescriptions)
	{
		ot::JsonObject entry;
		parameterDescription.addToJsonObject(entry, _allocator);
		quantityContainerEntryDescription.PushBack(entry, _allocator);
	}
	_object.AddMember("ParameterDescriptions",quantityContainerEntryDescription,_allocator);
}

void ot::QueryInformation::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	m_query = ot::json::getString(_object, "Query");
	m_projection = ot::json::getString(_object, "Projection");

	ot::ConstJsonArray quantityContainerEntryDescription = ot::json::getArray(_object, "ParameterDescriptions");

	for (uint32_t i = 0; i < quantityContainerEntryDescription.Size(); i++)
	{
		 ot::ConstJsonObject entry = ot::json::getObject(quantityContainerEntryDescription, i);
		 QuantityContainerEntryDescription parameterDescsr;
		 parameterDescsr.setFromJsonObject(entry);
		 m_parameterDescriptions.push_back(parameterDescsr);
	}
}