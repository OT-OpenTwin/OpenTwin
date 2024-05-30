//! @file ToolRuntimeHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// Toolkit header
#include "ToolManager.h"
#include "ToolRuntimeHandler.h"

// OpenTwin header
#include "OTCore/Logger.h"

ToolRuntimeHandler::ToolRuntimeHandler(otoolkit::Tool* _tool, ToolManager* _manager) 
	: m_tool(_tool), m_isRunning(false), m_manager(_manager)
{
	OTAssertNullptr(m_manager);
}

ToolRuntimeHandler::~ToolRuntimeHandler() {
	
}

void ToolRuntimeHandler::notifyDataChanged(void) {
	OTAssertNullptr(m_manager);
	m_manager->toolDataHasChanged(this);
}