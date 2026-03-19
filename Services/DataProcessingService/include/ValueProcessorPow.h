#pragma once
#include "ValueProcessor.h"
class ValueProcessorPow : public ValueProcessor
{
public:
	ValueProcessorPow(double _multiplier, double _exp);
	ot::Variable execute(const ot::Variable& _input) override;
	
private:
	const double m_multiplier;
	const double m_exponent;
};
