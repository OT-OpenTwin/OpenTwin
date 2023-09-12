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

#pragma warning(disable:4251)
namespace ot
{
	class OT_CORE_API_EXPORT Variable
	{
	public:
		Variable(float value);
		Variable(double value);
		Variable(int32_t value);
		Variable(int64_t value);
		Variable(bool value);
		Variable(const char* value);

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

	private:
		std::variant<int32_t, int64_t, bool, float, double, const char*> _value;
	};
	
}
