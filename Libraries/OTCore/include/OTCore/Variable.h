/*****************************************************************//**
 * \file   VariableType.h
 * \brief  Container for the usual data types. !ATTENTION! Currently it is required to sort a list of variables before calling unique. 
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#pragma once
#include <variant>
#include <stdint.h>
#include "OTCore/CoreTypes.h"
#include "OTCore/TypeNames.h"
#include <limits>
#include <algorithm>
#include <math.h>
#include "ComplexNumbers.h"

#pragma warning(disable:4251)
namespace ot
{

	class OT_CORE_API_EXPORT StringWrapper
	{
	public:
		StringWrapper(const char* ptr) {
			size_t length = strlen(ptr) + 1;
			_ptr = new char[length];
			strcpy_s(_ptr, length, ptr);
		}
		StringWrapper(const std::string& val)
		{
			_ptr = new char[val.size() + 1] {};
			val.copy(_ptr, val.size());
		}
		StringWrapper(std::string&& val)
		{
			_ptr = new char[val.size() + 1] {};
			memmove_s(_ptr, val.size(), val.c_str(), val.size());
		}

		StringWrapper(const StringWrapper& other)
		{
			size_t length = strlen(other._ptr) + 1;
			_ptr = new char[length] {};
			memcpy(_ptr, other._ptr, length);
		}
		StringWrapper(StringWrapper&& other) noexcept
		{
			size_t length = strlen(other._ptr) + 1;
			_ptr = new char[length];
			memmove_s(_ptr, length, other._ptr, length);
			other._ptr = nullptr;
		}
		StringWrapper& operator=(const char* ptr) {
			if (_ptr != nullptr) {
				delete[] _ptr;
				_ptr = nullptr;
			}

			size_t length = strlen(ptr) + 1;
			_ptr = new char[length];
			strcpy_s(_ptr, length, ptr);
			return *this;
		}
		StringWrapper& operator=(const StringWrapper& other)
		{
			if (_ptr != nullptr)
			{
				delete[] _ptr;
				_ptr = nullptr;
			}
			size_t length = strlen(other._ptr) + 1;
			_ptr = new char[length] {};
			memcpy(_ptr, other._ptr, length);
			return *this;
		}
		StringWrapper& operator=(StringWrapper&& other) noexcept
		{
			if (_ptr != nullptr)
			{
				delete[] _ptr;
				_ptr = nullptr;
			}
			size_t length = strlen(other._ptr) + 1;
			_ptr = new char[length] {};
			memmove_s(_ptr, length, other._ptr, length);
			other._ptr = nullptr;
			return *this;
		}

		bool operator==(const StringWrapper& other)
		{
			const int t = strcmp(_ptr, other._ptr);
			return t == 0;
		}
		bool operator<(const StringWrapper& other)
		{
			const int t = strcmp(_ptr, other._ptr);
			return t < 0;
		}
		bool operator>(const StringWrapper& other)
		{
			const int t = strcmp(_ptr, other._ptr);
			return t > 0;
		}

		operator const char* () const { return _ptr; }

		~StringWrapper()
		{
			if (_ptr != nullptr)
			{
				delete[] _ptr;
				_ptr = nullptr;
			}
		}
	private:
		char* _ptr = nullptr;
	};


	class OT_CORE_API_EXPORT Variable
	{
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
		Variable(const complex& value);
		Variable(complex&& value) noexcept;
		Variable(const Variable& other) = default;
		Variable(Variable&& other) = default;
		Variable& operator=(const Variable& other);
		Variable& operator=(Variable&& other) noexcept;

		void setValue(float value);
		void setValue(double value);
		void setValue(int32_t value);
		void setValue(int64_t value);
		void setValue(bool value);
		void setValue(const char* value);
		void setValue(const std::string& value);
		void setValue(std::string&& value);
		void setValue(const complex& _value);
		void setValue(complex&& _value);

		bool isFloat() const;
		bool isDouble() const;
		bool isInt32() const;
		bool isInt64() const;
		bool isBool() const;
		bool isConstCharPtr() const;
		bool isComplex() const;

		float getFloat() const;
		double getDouble() const;
		int32_t getInt32() const;
		int64_t getInt64() const;
		bool getBool() const;
		const char* getConstCharPtr() const;
		const complex getComplex() const;

		bool operator==(const Variable& other)const;
		bool operator>(const Variable& other)const;
		bool operator<(const Variable& other)const;

		std::string getTypeName()const;

	private:
		using variable_t = std::variant<int32_t, int64_t, bool, float, double ,std::string, complex>;
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
		variable_t _value;
	};
	
}
