// @otlicense
// File: Variable.h
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

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/TypeNames.h"
#include "OTCore/ComplexNumbers/ComplexNumberFormat.h"
#include "OTCore/ComplexNumbers/ComplexNumberConversion.h"

// std header
#include <limits>
#include <math.h>
#include <complex>
#include <variant>
#include <stdint.h>
#include <algorithm>

#pragma warning(disable:4251)
namespace ot {

	//! @brief Container for the usual data types.
	//! @note Currently it is required to sort a list of variables before calling unique.
	class OT_CORE_API_EXPORT Variable
	{
		OT_DECL_DEFCOPY(Variable)
		OT_DECL_DEFMOVE(Variable)
	public:

		Variable() {};
		Variable(float value);
		Variable(double value);
		Variable(int32_t value);
		Variable(int64_t value);
		Variable(bool value);
		Variable(const char* value);
		Variable(const std::string& value);
		Variable(std::string&& value) noexcept;
		Variable(const std::complex<double>& value);
		Variable(std::complex<double>&& value) noexcept;

		void setValue(float value);
		void setValue(double value);
		void setValue(int32_t value);
		void setValue(int64_t value);
		void setValue(bool value);
		void setValue(const char* value);
		void setValue(const std::string& value);
		void setValue(std::string&& value);
		void setValue(const std::complex<double>& _value);
		void setValue(std::complex<double>&& _value);
		
		//! @brief 
		//! @param _value Has to be of size 2. The order of the elements has to be the same as in the ComplexNumberConversion functions.
		void setValue(const ot::ComplexNumberDefinition& _complexNumberDefinition);

		bool isFloat() const;
		bool isDouble() const;
		bool isInt32() const;
		bool isInt64() const;
		bool isBool() const;
		bool isConstCharPtr() const;
		bool isComplex() const;
		bool isNumeric() const;

		float getFloat() const;
		double getDouble() const;
		int32_t getInt32() const;
		int64_t getInt64() const;
		bool getBool() const;
		const char* getConstCharPtr() const;
		const std::complex<double> getComplex() const;

		double toDouble() const;
		float toFloat() const;
		int64_t toInt64() const;
		int32_t toInt32() const;
		std::complex<double> toComplex() const;

		bool operator==(const Variable& other) const;
		bool operator!=(const Variable& other) const;
		bool operator>(const Variable& other) const;
		bool operator<(const Variable& other) const;
		
		Variable operator*(const Variable& o) const;
		Variable operator+(const Variable& o) const;
		Variable operator-(const Variable& o) const;
		Variable operator/(const Variable& o) const;

		std::string getTypeName() const;

	private:
		friend class VariableHelper;

		using variable_t = std::variant<int32_t, int64_t, bool, float, double ,std::string, std::complex<double>>;
		variable_t m_value;		
		
		inline bool DoubleCompare(const double& a, const double& b) const
		{
			constexpr const double epsilon = 1.0e-12; //std::numeric_limits<double>::epsilon()
			//Adaptive eps. Source: https://embeddeduse.com/2019/08/26/qt-compare-two-floats/
			if (abs(a - b) <= epsilon)
			{
				return true;
			}
			return abs(a - b) <= epsilon * (std::max)(abs(a), abs(b));
		}
		
		inline const bool FloatCompare(const float& a, const float& b) const
		{
			constexpr const float epsilon = 1.0e-6f; //std::numeric_limits<float>::epsilon()
			//Adaptive eps. Source: https://embeddeduse.com/2019/08/26/qt-compare-two-floats/
			if (abs(a - b) <= epsilon)
			{
				return true;
			}
			return abs(a - b) <= epsilon * (std::max)(abs(a), abs(b));
		}
	};
	
}
