#pragma once
#include "OTCore/Variable/Variable.h"
#include <memory>

class ValueProcessor 
{
public:
	ValueProcessor() = default;
	virtual ~ValueProcessor() = default;
	virtual ot::Variable execute(const ot::Variable& _input) = 0;
	virtual std::unique_ptr<ValueProcessor> inverse() const = 0;
	virtual ValueProcessor* createCopy() = 0;

};
