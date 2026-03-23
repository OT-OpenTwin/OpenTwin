// @otlicense

// OpenTwin header
#include "OTCore/ValueProcessing/ValueProcessorAdd.h"

static ot::ValueProcessor::Registrar<ot::ValueProcessorAdd> registrar(ot::ValueProcessorAdd::className());

ot::ValueProcessorAdd::ValueProcessorAdd(double _summand)
	: m_summand(_summand)
{}

ot::Variable ot::ValueProcessorAdd::execute(const ot::Variable & _input)
{
	return _input + m_summand;
}

ot::ValueProcessor* ot::ValueProcessorAdd::inverse() const
{
	return new ValueProcessorAdd(-m_summand);
}

void ot::ValueProcessorAdd::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	ValueProcessor::addToJsonObject(_jsonObject, _allocator);
	_jsonObject.AddMember("Summand", m_summand, _allocator);
}

void ot::ValueProcessorAdd::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	ValueProcessor::setFromJsonObject(_jsonObject);
	m_summand = json::getDouble(_jsonObject, "Summand");
}