// @otlicense

#include "OTCore/Variable.h"
#include <cassert>

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
{

}

ot::Variable::Variable(const std::string& value)
	:_value(value)
{
}


ot::Variable::Variable(std::string&& value) noexcept
{
	_value = std::move(value);
}

ot::Variable::Variable(const complex& value)
	:_value(value)
{}

ot::Variable::Variable(complex&& value) noexcept
	:_value(std::move(value))
{}

ot::Variable& ot::Variable::operator=(const Variable& other)
{
	_value = other._value;
	return *this;
}

ot::Variable& ot::Variable::operator=(Variable&& other) noexcept
{
	_value = std::move(other._value);
	return *this;
}

void ot::Variable::setValue(float value)
{
	_value = value;
}

void ot::Variable::setValue(double value)
{
	_value = value;
}

void ot::Variable::setValue(int32_t value)
{
	_value = value;
}

void ot::Variable::setValue(int64_t value)
{
	_value = value;
}

void ot::Variable::setValue(bool value)
{
	_value = value;
}

void ot::Variable::setValue(const char* value)
{
	_value = value;
}

void ot::Variable::setValue(const std::string& value)
{
	_value = value;
}

void ot::Variable::setValue(std::string&& value)
{
	_value = std::move(value);
}

void ot::Variable::setValue(const std::complex<double>& value)
{
	_value = value;
}

void ot::Variable::setValue(std::complex<double>&& value)
{
	_value = std::move(value);
}


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
	return std::holds_alternative<std::string>(_value);
}

bool ot::Variable::isComplex() const
{
	return std::holds_alternative<complex>(_value);
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
	return (std::get<std::string>(_value)).c_str();
}

const ot::complex ot::Variable::getComplex() const
{
	return std::get<complex>(_value);
}

bool ot::Variable::operator==(const Variable& other) const
{
	const bool equal =
		other.isBool() && this->isBool() && (other.getBool() == this->getBool()) ||
		other.isConstCharPtr() && this->isConstCharPtr() && (*other.getConstCharPtr() == *this->getConstCharPtr()) ||
		other.isDouble() && this->isDouble() && (DoubleCompare(other.getDouble(),this->getDouble())) ||
		other.isFloat() && this->isFloat() && (FloatCompare(other.getFloat(),this->getFloat())) ||
		other.isInt32() && this->isInt32() && (other.getInt32() == this->getInt32()) ||
		other.isInt64() && this->isInt64() && (other.getInt64() == this->getInt64());
	return equal;
	
}

bool ot::Variable::operator>(const Variable& other) const
{
	const bool larger =
		other.isBool() && this->isBool() && (this->getBool()> other.getBool()) ||
		other.isConstCharPtr() && this->isConstCharPtr() && (*this->getConstCharPtr()> *other.getConstCharPtr()) ||
		other.isDouble() && this->isDouble() && (this->getDouble()> other.getDouble()) ||
		other.isFloat() && this->isFloat() && (this->getFloat()> other.getFloat()) ||
		other.isInt32() && this->isInt32() && (this->getInt32() > other.getInt32()) ||
		other.isInt64() && this->isInt64() && (this->getInt64() > other.getInt64() );
	return larger;
}

bool ot::Variable::operator<(const Variable& other) const 
{
	const bool smaller =
		other.isBool() && this->isBool() && (this->getBool() < other.getBool()) ||
		other.isConstCharPtr() && this->isConstCharPtr() && (*this->getConstCharPtr() < *other.getConstCharPtr()) ||
		other.isDouble() && this->isDouble() && (this->getDouble() < other.getDouble()) ||
		other.isFloat() && this->isFloat() && (this->getFloat() < other.getFloat()) ||
		other.isInt32() && this->isInt32() && (this->getInt32() < other.getInt32()) ||
		other.isInt64() && this->isInt64() && (this->getInt64() < other.getInt64());
	return smaller;
}

std::string ot::Variable::getTypeName() const
{
	size_t index = _value.index();
	if (index == 0)
	{
		return ot::TypeNames::getInt32TypeName();
	}
	else if (index == 1)
	{
		return ot::TypeNames::getInt64TypeName();
	}
	else if (index == 2)
	{
		return ot::TypeNames::getBoolTypeName();
	}
	else if (index == 3)
	{
		return ot::TypeNames::getFloatTypeName();
	}
	else if (index == 4)
	{
		return ot::TypeNames::getDoubleTypeName();
	}
	else if (index == 5)
	{
		return ot::TypeNames::getStringTypeName();
	}
	else
	{
		assert(0);
		return "";
	}
}
