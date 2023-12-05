#pragma once

#include "OTCore/JSON.h"
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport) VariableToJSONConverter
	{
	public:
		rapidjson::Value operator() (Variable& value, JsonAllocator& jsonAllocator);
	};
}
