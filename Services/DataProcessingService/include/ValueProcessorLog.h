#pragma once
#include "ValueProcessor.h"

class ValueProcessorLog : public ValueProcessor
{
public:
	enum class SupportedBases
	{
		m_10,
		m_e
	};
	ValueProcessorLog(double _multiplier, SupportedBases _base);
	ot::Variable execute(const ot::Variable& _input) override;
	// Inherited via ValueProcessor
	std::unique_ptr<ValueProcessor> inverse() const override;
	
private:
	const double m_multiplier;
	SupportedBases m_base;

};
