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
