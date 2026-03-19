#include "ValueProcessorMultiply.h"

ValueProcessorMultiply::ValueProcessorMultiply(double _factor)
    :m_factor(_factor)
{
}

ot::Variable ValueProcessorMultiply::execute(const ot::Variable& _input)
{
    return ot::Variable();
}
