#pragma once

#include "OTCore/JSON.h"
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport) VariableToJSONConverter
	{
	public:
		ot::JsonValue operator() (const Variable& value, JsonAllocator& jsonAllocator);
		ot::JsonValue operator()(const std::list<Variable>& variables, JsonAllocator& emebeddingDocument);
	};
}
