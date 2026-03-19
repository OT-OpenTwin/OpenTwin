#pragma once
#include "ValueProcessor.h"

class ValueProcessorMultiply : public ValueProcessor
{
public:
	ValueProcessorMultiply(double _factor);
	ot::Variable execute(const ot::Variable& _input) override;
	
private:
	const double m_factor;

};