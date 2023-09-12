#include "openTwinCore/Variable.h"

ot::Variable::Variable(float value)
	:_value(value)
{}

ot::Variable::Variable(double value)
	:_value(value)
{}

ot::Variable::Variable(int32_t value)
	:_value(value)
{}

ot::Variable::Variable(int64_t value)
	:_value(value)
{}

ot::Variable::Variable(bool value)
	:_value(value)
{}

ot::Variable::Variable(const char* value)
	:_value(value)
{}

bool ot::Variable::isFloat() const
{
	return std::holds_alternative<float>(_value);
}

bool ot::Variable::isDouble() const
{
	return std::holds_alternative<double>(_value);
}

bool ot::Variable::isInt32() const
{
	return std::holds_alternative<int32_t>(_value);
}

bool ot::Variable::isInt64() const
{
	return std::holds_alternative<int64_t>(_value);
}

bool ot::Variable::isBool() const
{
	return std::holds_alternative<bool>(_value);
}

bool ot::Variable::isConstCharPtr() const
{
	return std::holds_alternative<const char*>(_value);
}

float ot::Variable::getFloat() const
{
	return std::get<float>(_value);
}

double ot::Variable::getDouble() const
{
	return std::get<double>(_value);
}

int32_t ot::Variable::getInt32() const
{
	return std::get<int32_t>(_value);
}

int64_t ot::Variable::getInt64() const
{
	return std::get<int64_t>(_value);
}

bool ot::Variable::getBool() const
{
	return std::get<bool>(_value);
}

const char* ot::Variable::getConstCharPtr() const
{
	return std::get<const char*>(_value);
}
