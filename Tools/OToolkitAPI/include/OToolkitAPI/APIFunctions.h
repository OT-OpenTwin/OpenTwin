//! @file APIFunctions.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// std header
#include <list>

#define OTOOLKIT_CreateToolsFunctionName "CreateTools"

namespace otoolkit {

	class Tool;

	//! \brief Import tools function prototype.
	typedef void(*ImportToolsFunction)(std::list<Tool*>&);
}
