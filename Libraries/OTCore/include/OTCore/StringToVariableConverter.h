#pragma once
#include "Variable.h"
#include <string>

namespace ot
{
	/// <summary>
	/// Transforms a string into a json and subsequently into ot::variable.
	/// Strings values that are numbers are only recognised as strings if " characters are added before and after the string value. Example: string = \"4\";
	/// Boolean values have to be "true" or "false"
	/// </summary>
	class __declspec(dllexport)  StringToVariableConverter
	{
	public:
		Variable operator() (const std::string& _value, const char _decimalSeparator);

		//! @brief Takes a numeric value as string and normalises the floating point representation. Removes whitespaces, thousands separators and ensures point as decimal separator.
		//! @param _value Numeric value as string, which will be alternated
		//! @param _decimalSeparator The decimal separator that is used in the string. Can be a ',' or '.'.
		//! @return true if the normalisation worked, false if > 1 decimal separator where found.
		bool normaliseNumericString(std::string& _value, const char _decimalSeparator);
	private:
		void removeThousandsSeparators(std::string& _value, const char _thousandsSeparator);
		void removeWhitespaces(std::string& _value);
		void removeControlCharacters(std::string& _value);
		void ensurePointAsDecimalSeparator(std::string& _value);
		Variable convertIfFitsNumericalValue(const std::string& _value);
	};
}