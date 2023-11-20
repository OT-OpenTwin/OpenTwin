//! @file ToolRuntimeHandler.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

namespace otoolkit { class Tool; };

class ToolRuntimeHandler {
	OT_DECL_NOCOPY(ToolRuntimeHandler)
public:
	ToolRuntimeHandler(otoolkit::Tool* _tool);
	virtual ~ToolRuntimeHandler();

	otoolkit::Tool* tool(void) { return m_tool; };

	void setRunning(void) { m_isRunning = true; };
	void setStopped(void) { m_isRunning = false; };
	bool isRunning(void) const { return m_isRunning; };

private:
	otoolkit::Tool* m_tool;
	bool m_isRunning;

	ToolRuntimeHandler() = delete;
};