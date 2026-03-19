#include "ValueProcessorPow.h"
#include "OTCore/Variable/VariableHelper.h"
ValueProcessorPow::ValueProcessorPow(double _multiplier, double _exp)
	:m_exponent(_exp), m_multiplier(_multiplier)
{}

ot::Variable ValueProcessorPow::execute(const ot::Variable & _input)
{
	ot::Variable temp =	ot::VariableHelper::pow(_input, m_exponent);
	temp = temp * m_multiplier;
	return temp;
}

std::unique_ptr<ValueProcessor> ValueProcessorPow::inverse() const
{
	return std::make_unique<ValueProcessorPow>(1/m_multiplier, 1.0 / m_exponent);
}
