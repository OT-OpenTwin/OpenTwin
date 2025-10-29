// @otlicense

/*****************************************************************//**
 * \file   TemplateTypeName.h
 * \brief  Template for getting a type name. Some type names are not system independent, hence the need to create a standardised way for this project.
 * 
 * \author Wagner
 * \date   February 2023
 *********************************************************************/

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