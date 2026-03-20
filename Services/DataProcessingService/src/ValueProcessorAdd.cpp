#include "ValueProcessorAdd.h"

ValueProcessorAdd::ValueProcessorAdd(double _summand)
	:m_summand(_summand)
{}

ot::Variable ValueProcessorAdd::execute(const ot::Variable & _input)
{
	return  _input + m_summand;
}

std::unique_ptr<ValueProcessor> ValueProcessorAdd::inverse() const
{
	auto add = std::make_unique<ValueProcessorAdd>(-m_summand);
	return add;
}

ValueProcessor* ValueProcessorAdd::createCopy()
{
	auto newProcessor = std::make_unique<ValueProcessorAdd>(m_summand);
	return newProcessor.release();
}
