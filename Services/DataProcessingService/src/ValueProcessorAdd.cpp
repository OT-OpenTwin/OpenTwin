#include "ValueProcessorAdd.h"

ValueProcessorAdd::ValueProcessorAdd(ot::Variable _summand)
	:m_summand(_summand)
{}

ot::Variable ValueProcessorAdd::execute(const ot::Variable & _input)
{
	return m_summand + _input;
}