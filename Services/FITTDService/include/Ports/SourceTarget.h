#pragma once
#include <map>
#include <string>
enum sourceType { e, h };
enum axis { x_axis, y_axis, z_axis };

namespace port
{
	static std::map<axis, std::string> axisName = { {x_axis, "X-Axis"}, {y_axis, "Y-Axis"}, {z_axis, "Z-Axis"} };
	static std::map<axis, std::string> axisAbbreviation = { {x_axis, "x"}, {y_axis, "y"}, {z_axis, "z"} };
}

