/*****************************************************************//**
 * \file   TypeNames.h
 * \brief  Some type names are not system independent, hence the need to create a standardised way for this project. 
 * This class provides the names themselve as a central point of access. * 
 * 
 * \author Wagner
 * \date   February 2023
 *********************************************************************/

#pragma once

// std header
#include <string>
#include <stdint.h>

namespace ot
{
	class TypeNames
	{
	public:
		static std::string getInt32TypeName() { return "int32"; };
		static std::string getInt64TypeName() { return "int64"; };
		static std::string getStringTypeName() { return "string"; };
		static std::string getDoubleTypeName() { return "double"; };
		static std::string getFloatTypeName() { return "float"; };
		static std::string getCharTypeName() { return "char"; };
	};
}