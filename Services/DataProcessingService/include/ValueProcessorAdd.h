#pragma once
#include "ValueProcessor.h"

class ValueProcessorAdd : public ValueProcessor
{
public:
	ValueProcessorAdd(ot::Variable _summand);
	ot::Variable execute(const ot::Variable& _input) override;
	
private:
	const ot::Variable m_summand;

};
