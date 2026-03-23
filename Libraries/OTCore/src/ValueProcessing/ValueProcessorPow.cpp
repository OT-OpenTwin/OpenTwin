// @otlicense

// OpenTwin header
#include "OTCore/ValueProcessing/ValueProcessorLog.h"
#include "OTCore/ValueProcessing/ValueProcessorPow.h"
#include "OTCore/Variable/VariableHelper.h"

static ot::ValueProcessor::Registrar<ot::ValueProcessorPow> registrar(ot::ValueProcessorPow::className());

ot::ValueProcessorPow::ValueProcessorPow(double _multiplier, double _exp)
	: m_exponent(_exp), m_multiplier(_multiplier)
{}

ot::Variable ot::ValueProcessorPow::execute(const ot::Variable& _input)
{
	ot::Variable temp =	ot::VariableHelper::pow(_input, m_exponent);
	temp = temp * m_multiplier;
	return temp;
}

ot::ValueProcessor* ot::ValueProcessorPow::inverse() const
{
	OTAssert(m_exponent != 0, "Exponent must not be zero for inverse of power operation.");
	return new ValueProcessorPow(1. / m_multiplier, 1. / m_exponent);
}

void ot::ValueProcessorPow::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	ValueProcessor::addToJsonObject(_jsonObject, _allocator);
	_jsonObject.AddMember("Multiplier", m_multiplier, _allocator);
	_jsonObject.AddMember("Exponent", m_exponent, _allocator);
}

void ot::ValueProcessorPow::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	ValueProcessor::setFromJsonObject(_jsonObject);
	m_multiplier = json::getDouble(_jsonObject, "Multiplier");
	m_exponent = json::getDouble(_jsonObject, "Exponent");
}
