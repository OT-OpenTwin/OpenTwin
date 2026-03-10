// @otlicense
// File: Variable.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"
#include "OTCore/ComplexNumbers/ComplexNumberConversion.h"
#include "OTCore/ComplexNumbers/ComplexNumberDefinition.h"
#include "OTCore/Variable/VariableHelper.h"

ot::Variable::Variable(float value)
	: m_value(value)
{}

ot::Variable::Variable(double value)
	: m_value(value)
{}

ot::Variable::Variable(int32_t value)
	: m_value(value)
{}

ot::Variable::Variable(int64_t value)
	: m_value(value)
{}

ot::Variable::Variable(bool value)
	: m_value(value)
{}

ot::Variable::Variable(const char* value)
	: m_value(value)
{}

ot::Variable::Variable(const std::string& value)
	: m_value(value)
{}

ot::Variable::Variable(std::string&& value) noexcept
{
	m_value = std::move(value);
}

ot::Variable::Variable(const std::complex<double>& value)
	: m_value(value)
{}

ot::Variable::Variable(std::complex<double>&& value) noexcept
	: m_value(std::move(value))
{}

void ot::Variable::setValue(float value)
{
	m_value = value;
}

void ot::Variable::setValue(double value)
{
	m_value = value;
}

void ot::Variable::setValue(int32_t value)
{
	m_value = value;
}

void ot::Variable::setValue(int64_t value)
{
	m_value = value;
}

void ot::Variable::setValue(bool value)
{
	m_value = value;
}

void ot::Variable::setValue(const char* value)
{
	m_value = value;
}

void ot::Variable::setValue(const std::string& value)
{
	m_value = value;
}

void ot::Variable::setValue(std::string&& value)
{
	m_value = std::move(value);
}

void ot::Variable::setValue(const std::complex<double>& value)
{
	m_value = value;
}

void ot::Variable::setValue(std::complex<double>&& value)
{
	m_value = std::move(value);
}

void ot::Variable::setValue(const ot::ComplexNumberDefinition& _complexNumberDefinition)
{
	if (_complexNumberDefinition.m_format == ot::ComplexNumberFormat::Cartesian)
	{
		m_value = std::complex<double>(_complexNumberDefinition.m_firstValue, _complexNumberDefinition.m_secondValue);
	}
	else
	{
		m_value = ot::ComplexNumberConversion::polarToCartesian(_complexNumberDefinition);
	}
}


bool ot::Variable::isFloat() const
{
	return std::holds_alternative<float>(m_value);
}

bool ot::Variable::isDouble() const
{
	return std::holds_alternative<double>(m_value);
}

bool ot::Variable::isInt32() const
{
	return std::holds_alternative<int32_t>(m_value);
}

bool ot::Variable::isInt64() const
{
	return std::holds_alternative<int64_t>(m_value);
}

bool ot::Variable::isBool() const
{
	return std::holds_alternative<bool>(m_value);
}

bool ot::Variable::isConstCharPtr() const
{
	return std::holds_alternative<std::string>(m_value);
}

bool ot::Variable::isComplex() const
{
	return std::holds_alternative<std::complex<double>>(m_value);
}

bool ot::Variable::isNumeric() const
{
	return isFloat() || isDouble() || isInt32() || isInt64() || isComplex();
}

float ot::Variable::getFloat() const
{
	return std::get<float>(m_value);
}

double ot::Variable::getDouble() const
{
	return std::get<double>(m_value);
}

int32_t ot::Variable::getInt32() const
{
	return std::get<int32_t>(m_value);
}

int64_t ot::Variable::getInt64() const
{
	return std::get<int64_t>(m_value);
}

bool ot::Variable::getBool() const
{
	return std::get<bool>(m_value);
}

const char* ot::Variable::getConstCharPtr() const
{
	return (std::get<std::string>(m_value)).c_str();
}

const std::complex<double> ot::Variable::getComplex() const
{
	return std::get<std::complex<double>>(m_value);
}

double ot::Variable::toDouble() const
{
	if (isDouble())
	{
		return getDouble();
	}
	else if (isFloat())
	{
		return static_cast<double>(getFloat());
	}
	else if (isInt64())
	{
		return static_cast<double>(getInt64());
	}
	else if (isInt32())
	{
		return static_cast<double>(getInt32());
	}
	else
	{
		throw std::runtime_error("Variable: cannot convert to double: " + getTypeName());
	}
}

float ot::Variable::toFloat() const
{
	if (isFloat())
	{
		return getFloat();
	}
	else if (isDouble())
	{
		return static_cast<float>(getDouble());
	}
	else if (isInt64())
	{
		return static_cast<float>(getInt64());
	}
	else if (isInt32())
	{
		return static_cast<float>(getInt32());
	}
	else
	{
		throw std::runtime_error("Variable: cannot convert to float: " + getTypeName());
	}
}

int64_t ot::Variable::toInt64() const
{
	if (isInt64())
	{
		return getInt64();
	}
	else if (isInt32())
	{
		return static_cast<int64_t>(getInt32());
	}
	else if (isDouble())
	{
		return static_cast<int64_t>(getDouble());
	}
	else if (isFloat())
	{
		return static_cast<int64_t>(getFloat());
	}
	else
	{
		throw std::runtime_error("Variable: cannot convert to int64: " + getTypeName());
	}
}

int32_t ot::Variable::toInt32() const
{
	if (isInt32())
	{
		return getInt32();
	}
	else if (isInt64())
	{
		return static_cast<int32_t>(getInt64());
	}
	else if (isDouble())
	{
		return static_cast<int32_t>(getDouble());
	}
	else if (isFloat())
	{
		return static_cast<int32_t>(getFloat());
	}
	else
	{
		throw std::runtime_error("Variable: cannot convert to int32: " + getTypeName());
	}
}

std::complex<double> ot::Variable::toComplex() const
{
	if (isComplex())
	{
		return getComplex();
	}
	else if (isDouble())
	{
		return std::complex<double>(getDouble());
	}
	else if (isFloat())
	{
		return std::complex<double>(static_cast<double>(getFloat()));
	}
	else if (isInt64())
	{
		return std::complex<double>(static_cast<double>(getInt64()));
	}
	else if (isInt32())
	{
		return std::complex<double>(static_cast<double>(getInt32()));
	}
	else
	{
		throw ("Trying to cast a non numeric variable to complex");
	}
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

bool ot::Variable::operator!=(const Variable& other) const {
	return !(*this == other);
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

ot::Variable ot::Variable::operator*(const Variable& o) const
{
	return VariableHelper::applyNumericOp(*this, o, [](auto a, auto b) { return a * b; });
}

ot::Variable ot::Variable::operator+(const Variable& o) const
{
	return VariableHelper::applyNumericOp(*this, o, [](auto a, auto b) { return a + b; });
}

ot::Variable ot::Variable::operator-(const Variable& o) const
{
	return VariableHelper::applyNumericOp(*this, o, [](auto a, auto b) { return a - b; });
}

ot::Variable ot::Variable::operator/(const Variable& o) const
{
	return VariableHelper::applyNumericOp(*this, o, [](auto a, auto b) { return a / b; });
}

std::string ot::Variable::getTypeName() const
{
	const size_t index = m_value.index();
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
		OT_LOG_EAS("[FATAL] Unknown variant index (" + std::to_string(index) + ")");
		return "";
	}
}
