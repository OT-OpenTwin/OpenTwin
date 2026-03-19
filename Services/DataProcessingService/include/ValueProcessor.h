#pragma once
#include "OTCore/Variable/Variable.h"
#include <memory>

class ValueProcessor 
{
public:
	ValueProcessor() = default;
	virtual ~ValueProcessor() = default;
	virtual ot::Variable execute(const ot::Variable& _input) = 0;
	ValueProcessor* getNext() const { return m_next; }
	void setNext(ValueProcessor& _next) { m_next = &_next; }
	virtual std::unique_ptr<ValueProcessor> inverse() const = 0;

private:
	ValueProcessor* m_next = nullptr;
};
