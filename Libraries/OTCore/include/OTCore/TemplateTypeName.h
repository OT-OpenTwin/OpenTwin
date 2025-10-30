// @otlicense

#pragma once

// std header
#include <string>

#include "OTCore/TypeNames.h"

namespace ot
{
	template<class T>
	class TemplateTypeName
	{
		public:
			static std::string getTypeName();
	};
}

#include "OTCore/TemplateTypeName.hpp"