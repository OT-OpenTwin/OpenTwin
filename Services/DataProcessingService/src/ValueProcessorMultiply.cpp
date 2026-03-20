#include "ValueProcessorMultiply.h"

ValueProcessorMultiply::ValueProcessorMultiply(double _factor)
    :m_factor(_factor)
{}

ot::Variable ValueProcessorMultiply::execute(const ot::Variable& _input)
{
    return _input * m_factor;
}

std::unique_ptr<ValueProcessor> ValueProcessorMultiply::inverse() const
{
    return std::make_unique<ValueProcessorMultiply>(1/m_factor);
}

ValueProcessor* ValueProcessorMultiply::createCopy()
{
    auto newProcessor = std::make_unique<ValueProcessorMultiply>(m_factor);
    return newProcessor.release();
}


