// @otlicense

// OpenTwin header
#include "OTCore/ValueProcessing/ValueProcessorMultiply.h"

static ot::ValueProcessor::Registrar<ot::ValueProcessorMultiply> registrar(ot::ValueProcessorMultiply::className());

ot::ValueProcessorMultiply::ValueProcessorMultiply(double _factor)
    : m_factor(_factor)
{}

ot::Variable ot::ValueProcessorMultiply::execute(const ot::Variable& _input)
{
    return _input * m_factor;
}

ot::ValueProcessor* ot::ValueProcessorMultiply::inverse() const
{
    return new ValueProcessorMultiply(1. / m_factor);
}

void ot::ValueProcessorMultiply::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
    ValueProcessor::addToJsonObject(_jsonObject, _allocator);
    _jsonObject.AddMember("Factor", m_factor, _allocator);
}

void ot::ValueProcessorMultiply::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
    ValueProcessor::setFromJsonObject(_jsonObject);
    m_factor = json::getDouble(_jsonObject, "Factor");
}
