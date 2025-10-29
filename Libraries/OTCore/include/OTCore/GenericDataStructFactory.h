// @otlicense

#pragma once
#include <memory>
#include "GenericDataStruct.h"
#include "JSON.h"

namespace ot
{
	class __declspec(dllexport) GenericDataStructFactory
	{
	public:
		static ot::GenericDataStruct* Create(const ot::ConstJsonObject& object);
	};
}