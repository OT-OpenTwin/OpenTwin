// @otlicense

// Toolkit header
#include "ToolManager.h"
#include "ToolRuntimeHandler.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"

ToolRuntimeHandler::ToolRuntimeHandler(otoolkit::Tool* _tool, ToolManager* _manager) 
	: m_tool(_tool), m_manager(_manager)
{
	OTAssertNullptr(m_manager);
}

ToolRuntimeHandler::~ToolRuntimeHandler() {
	
}

void ToolRuntimeHandler::notifyDataChanged(void) {
	OTAssertNullptr(m_manager);
	m_manager->toolDataHasChanged(this);
}