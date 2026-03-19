#pragma once
#include "ValueProcessor.h"

class ValueProcessorAdd : public ValueProcessor
{
public:
	ValueProcessorAdd(double _summand);
	ot::Variable execute(const ot::Variable& _input) override;
	// Inherited via ValueProcessor
	std::unique_ptr<ValueProcessor> inverse() const override;
	
private:
	const double m_summand;



};
