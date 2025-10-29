// @otlicense

#pragma once
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport) VariableToBSONStringConverter
	{
	public:
		std::string operator()(const ot::Variable& variable);
	};

}
