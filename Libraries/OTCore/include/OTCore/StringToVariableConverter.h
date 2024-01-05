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

		Variable operator() (const std::string& value);
	};
}