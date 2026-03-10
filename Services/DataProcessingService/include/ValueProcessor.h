#pragma once
#include "OTCore/Variable.h"
class ValueProcessor 
{
public:
	ValueProcessor() = default;
	virtual ~ValueProcessor() = default;
	virtual ot::Variable execute(const ot::Variable& _input) = 0;
	virtual ot::Variable executeInverse(const ot::Variable& _input) = 0;
	ValueProcessor* getNext() const { return m_next; }
	void setNext(ValueProcessor& _next) { m_next = &_next; }
private:
	ValueProcessor* m_next = nullptr;
};
