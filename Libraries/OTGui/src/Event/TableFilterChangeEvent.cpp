// @otlicense

// OpenTwin header
#include "OTGui/Event/TableFilterChangeEvent.h"

ot::TableFilterChangeEvent::TableFilterChangeEvent(const BasicEntityInformation& _basicEntityInformation)
	: m_tableInformation(_basicEntityInformation)
{}

ot::TableFilterChangeEvent::TableFilterChangeEvent(BasicEntityInformation&& _basicEntityInformation) noexcept
	: m_tableInformation(std::move(_basicEntityInformation))
{}

ot::TableFilterChangeEvent::TableFilterChangeEvent(const ConstJsonObject& _jsonObject)
{
	setFromJsonObject(_jsonObject);
}

void ot::TableFilterChangeEvent::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	GuiEvent::addToJsonObject(_object, _allocator);
	
	_object.AddMember("Table", JsonObject(m_tableInformation, _allocator), _allocator);

	JsonArray filterDescArr;
	for (const auto& filterDesc : m_filterDescriptions)
	{
		filterDescArr.PushBack(JsonObject(filterDesc, _allocator), _allocator);
	}
	_object.AddMember("filterDescriptions", filterDescArr, _allocator);
}

void ot::TableFilterChangeEvent::setFromJsonObject(const ConstJsonObject& _object)
{
	GuiEvent::setFromJsonObject(_object);
	
	m_tableInformation.setFromJsonObject(json::getObject(_object, "Table"));

	m_filterDescriptions.clear();
	for (const ConstJsonObject& filterDescObj : json::getObjectList(_object, "filterDescriptions"))
	{
		ValueComparisonDescription filterDesc;
		filterDesc.setFromJsonObject(filterDescObj);
		m_filterDescriptions.push_back(std::move(filterDesc));
	}
}
