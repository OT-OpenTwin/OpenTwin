#pragma once
#include "OTCore/rJSONHelper.h"
#include "OTCore/rJSON.h"
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport)  JSONToVariableConverter
	{
	public:
		Variable operator() (rapidjson::Value& value);
	};
}
