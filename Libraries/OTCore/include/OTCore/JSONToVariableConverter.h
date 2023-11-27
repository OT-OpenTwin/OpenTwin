#pragma once
#include "OTCore/JSON.h"
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport)  JSONToVariableConverter
	{
	public:
		Variable operator() (JsonValue& value);
	};
}
