#pragma once
#include <string>

namespace ot
{
	template <class T>
	struct Variable
	{
		std::string name;
		std::string type;
		T value;
	};
}