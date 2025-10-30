// @otlicense

#pragma once

// std header
#include <list>

#define OTOOLKIT_CreateToolsFunctionName "CreateTools"

namespace otoolkit {

	class Tool;

	//! \brief Import tools function prototype.
	typedef void(*ImportToolsFunction)(std::list<Tool*>&);
}
