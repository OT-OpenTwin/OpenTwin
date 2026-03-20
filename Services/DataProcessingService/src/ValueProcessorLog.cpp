#include "ValueProcessorLog.h"
#include "OTCore/Variable/VariableHelper.h"
#include "ValueProcessorPow.h"
ValueProcessorLog::ValueProcessorLog(double _multiplier, SupportedBases _base)
	: m_multiplier(_multiplier), m_base(_base)
{
}

ot::Variable ValueProcessorLog::execute(const ot::Variable& _input)
{
	ot::Variable value;
	if (m_base == SupportedBases::m_10)
	{
		 value = ot::VariableHelper::log10(_input);
	}
	else if (m_base == SupportedBases::m_e)
	{
		value = ot::VariableHelper::ln(_input);
	}
	
	value = value * m_multiplier;
	return value;
}

std::unique_ptr<ValueProcessor> ValueProcessorLog::inverse() const
{
	constexpr double e = 2.71828182845904523536;
	if (m_base == SupportedBases::m_10)
	{
		return std::make_unique<ValueProcessorPow>(m_multiplier, 10);
	}
	else if (m_base == SupportedBases::m_e)
	{
		return std::make_unique<ValueProcessorPow>(m_multiplier, e);
	}
	else
	{
		assert(false);
		throw std::exception("Not supported base in log operation.");
	}
	
}

ValueProcessor* ValueProcessorLog::createCopy()
{
	auto newProcessor = std::make_unique<ValueProcessorLog>(m_multiplier, m_base);
	return newProcessor.release();
}
