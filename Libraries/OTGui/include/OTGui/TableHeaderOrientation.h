#pragma once
#include <stdint.h>
#include <string>

namespace ot
{
	enum class TableHeaderOrientation : uint32_t
	{
		horizontal = 0,
		vertical = 1,
		none = 2
	};

	__declspec(dllexport) std::string toString(const ot::TableHeaderOrientation& _orientation);
}
