#pragma once
#include "ValueProcessor.h"

class ValueProcessorAdd : public ValueProcessor
{
public:
	ValueProcessorAdd(double _summand);
	ot::Variable execute(const ot::Variable& _input) override;
	// Inherited via ValueProcessor
	std::unique_ptr<ValueProcessor> inverse() const override;
	ValueProcessor* createCopy() override;
private:
	const double m_summand;



};
