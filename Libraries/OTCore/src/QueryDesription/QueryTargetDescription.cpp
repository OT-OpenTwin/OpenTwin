#include "OTCore//QueryDescription/QueryTargetDescription.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"

ot::QueryTargetDescription::QueryTargetDescription(const TupleInstance& _tupleInstance, const std::string& _mongoDBFieldName, const std::string& _targetLabel)
	: m_tupleDescription(_tupleInstance),m_mongoDBFieldName(_mongoDBFieldName),m_targetLabel(_targetLabel)
{}

void ot::QueryTargetDescription::setTupleInstance(const TupleInstance& _tupleInstance)
{
	m_tupleDescription = _tupleInstance;
}

void ot::QueryTargetDescription::setMongoDBFieldName(const std::string& _mongoDBFieldName)
{
	m_mongoDBFieldName = _mongoDBFieldName;
}

void ot::QueryTargetDescription::setTargetLabel(const std::string& _targetLabel)
{
	m_targetLabel = _targetLabel;
}

const std::string& ot::QueryTargetDescription::getLabel() const
{
	return m_targetLabel;
}

const std::string& ot::QueryTargetDescription::getMongoDBFieldName() const
{
	return m_mongoDBFieldName;
}

const ot::TupleInstance& ot::QueryTargetDescription::getTupleInstance() const
{
	return m_tupleDescription;
}

void ot::QueryTargetDescription::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{	
	_jsonObject.AddMember("mongoField", ot::JsonString(m_mongoDBFieldName, _allocator), _allocator);
	_jsonObject.AddMember("TargetLabel", ot::JsonString(m_targetLabel, _allocator), _allocator);
	ot::JsonObject obj;
	m_tupleDescription.addToJsonObject(obj, _allocator);
	_jsonObject.AddMember("Tuple", obj, _allocator);
}

void ot::QueryTargetDescription::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	m_mongoDBFieldName = ot::json::getString(_jsonObject, "mongoField");
	m_targetLabel = ot::json::getString(_jsonObject, "TargetLabel");
	ot::ConstJsonObject tuple = ot::json::getObject(_jsonObject, "Tuple");
	m_tupleDescription.setFromJsonObject(tuple);
}
