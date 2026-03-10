#include "OTCore//QueryDescription/QueryDescription.h"

ot::QueryDescription::QueryDescription(const ot::ValueComparisonDescription& _comparisonDescription, const QueryTargetDescription& _queryComparison)
	: m_comparisonDescription(_comparisonDescription), m_queryTargetDescription(_queryComparison)
{}

void ot::QueryDescription::setQueryTargetDescription(const QueryTargetDescription& _queryTargetDescription)
{
	m_queryTargetDescription = _queryTargetDescription;
}

void ot::QueryDescription::setComparisonDescription(const ot::ValueComparisonDescription& _valueComparison)
{
	m_comparisonDescription = _valueComparison;
}

const ot::QueryTargetDescription& ot::QueryDescription::getQueryTargetDescription() const
{
	return m_queryTargetDescription;
}

const ot::ValueComparisonDescription& ot::QueryDescription::getValueComparisonDescription() const
{
	return m_comparisonDescription;
}

void ot::QueryDescription::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	ot::JsonObject comparisonDesc;
	m_comparisonDescription.addToJsonObject(comparisonDesc, _allocator);
	_jsonObject.AddMember("comparison", comparisonDesc,_allocator);

	ot::JsonObject targetDescription;
	m_queryTargetDescription.addToJsonObject(targetDescription, _allocator);
	_jsonObject.AddMember("targetDesc", targetDescription, _allocator);
}

void ot::QueryDescription::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	ot::ConstJsonObject compObj = ot::json::getObject(_jsonObject, "comparison");
	m_comparisonDescription.setFromJsonObject(compObj);

	ot::ConstJsonObject targetObj = ot::json::getObject(_jsonObject, "targetDesc");
	m_queryTargetDescription.setFromJsonObject(targetObj);
}
