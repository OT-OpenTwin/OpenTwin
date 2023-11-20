#pragma once

#include "OTCore/rJSONHelper.h"
#include "OTCore/rJSON.h"
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport) VariableToJSONConverter
	{
	public:
		rapidjson::Value operator() (Variable& value, OT_rJSON_doc& emebeddingDocument);
	};
}
