//! @file ToolRuntimeHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// Toolkit header
#include "ToolRuntimeHandler.h"

ToolRuntimeHandler::ToolRuntimeHandler(otoolkit::Tool* _tool) : m_tool(_tool), m_isRunning(false) {

}

ToolRuntimeHandler::~ToolRuntimeHandler() {
	//if (m_tool) delete m_tool;
}
