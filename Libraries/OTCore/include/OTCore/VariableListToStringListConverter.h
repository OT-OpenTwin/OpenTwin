#pragma once
#include "Variable.h"
#include <list>
#include <string>

namespace ot
{
	class __declspec(dllexport) VariableListToStringListConverter
	{
	public:
		std::list<std::string> operator() (const std::list<Variable>& values);
	};
}
