#include "ValueProcessorLog.h"

ValueProcessorLog::ValueProcessorLog(double _multiplier, double _base)
	: m_multiplier(_multiplier), m_base(_base)
{
}

ot::Variable ValueProcessorLog::execute(const ot::Variable& _input)
{
	return ot::Variable();
}
