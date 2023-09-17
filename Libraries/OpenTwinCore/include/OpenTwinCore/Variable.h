/*****************************************************************//**
 * \file   VariableType.h
 * \brief  Classes that convert from the JSON format that is being used for the inter service communication to the internally used variant and vice versa.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#pragma once
#include <variant>
#include <stdint.h>
#include "OpenTwinCore/CoreAPIExport.h"
#include "openTwinCore/TypeNames.h"

#pragma warning(disable:4251)
namespace ot
{

	class OT_CORE_API_EXPORT StringWrapper
	{
	public:
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
		StringWrapper(StringWrapper&& other)
		{
			size_t length = strlen(other._ptr) + 1;
			_ptr = new char[length];
			memmove_s(_ptr, length, other._ptr, length);
			other._ptr = nullptr;
		}
		StringWrapper& operator=(StringWrapper& other)
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
		StringWrapper& operator=(StringWrapper&& other)
		{
			if (_ptr != nullptr)
			{
				delete[] _ptr;
				_ptr = nullptr;
			}
			size_t length = strlen(other._ptr) + 1;
			memmove_s(_ptr, length, other._ptr, length - 1);
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
		Variable(float value);
		Variable(double value);
		Variable(int32_t value);
		Variable(int64_t value);
		Variable(bool value);
		Variable(const char* value);
		Variable(const std::string& value);
		Variable(std::string&& value);
		
		Variable(const Variable& other) = default;
		Variable(Variable&& other) = default;
		Variable& operator=(const Variable& other);
		Variable& operator=(Variable&& other);



		bool isFloat() const;
		bool isDouble() const;
		bool isInt32() const;
		bool isInt64() const;
		bool isBool() const;
		bool isConstCharPtr() const;

		float getFloat() const;
		double getDouble() const;
		int32_t getInt32() const;
		int64_t getInt64() const;
		bool getBool() const;
		const char* getConstCharPtr() const;

		bool operator==(const Variable& other)const;
		bool operator>(const Variable& other)const;
		bool operator<(const Variable& other)const;

		std::string getTypeName()const;

	private:
		using variable_t = std::variant<int32_t, int64_t, bool, float, double ,StringWrapper>;
		variable_t _value;
	};
	
}
