// @otlicense

#pragma once
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport) VariableToStringConverter
	{
	public:
		std::string operator() (const Variable& value);
	};
}
