#pragma once
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/rJSON.h"
#include "Variable.h"

namespace ot
{
	class VariableToJSONConverter
	{
	public:
		__declspec(dllexport) rapidjson::Value operator() (Variable& value, OT_rJSON_doc& emebeddingDocument);
	};
}
