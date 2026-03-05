#pragma once
#include "OTCore/Variable.h"
class ValueProcessor 
{
public:
	ValueProcessor() = default;
	virtual ~ValueProcessor() = default;
	ot::Variable execute(const ot::Variable& _input);
	const ValueProcessor* getNext();
	void setNext(const ValueProcessor& _next);
};
