#pragma once
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/rJSON.h"
#include "Variable.h"

namespace ot
{
	class JSONToVariableConverter
	{
	public:
		__declspec(dllexport) Variable operator() (rapidjson::Value& value);
	};
}
