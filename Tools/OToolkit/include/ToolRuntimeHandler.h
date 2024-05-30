//! @file ToolRuntimeHandler.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// API header
#include "OToolkitAPI/Tool.h"
#include "OToolkitAPI/ToolWidgets.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"

class ToolManager;

class ToolRuntimeHandler {
	OT_DECL_NOCOPY(ToolRuntimeHandler)
public:
	ToolRuntimeHandler(otoolkit::Tool* _tool, ToolManager* _manager);
	virtual ~ToolRuntimeHandler();

	otoolkit::Tool* getTool(void) { return m_tool; };

	void setRunning(void) { m_isRunning = true; };
	void setStopped(void) { m_isRunning = false; };
	bool isRunning(void) const { return m_isRunning; };

	void setToolWidgets(const otoolkit::ToolWidgets& _toolWidgets) { m_toolWidgets = _toolWidgets; };
	const otoolkit::ToolWidgets& getToolWidgets(void) const { return m_toolWidgets; };
	otoolkit::ToolWidgets& getToolWidgets(void) { return m_toolWidgets; };

	void notifyDataChanged(void);

private:
	ToolManager* m_manager;
	otoolkit::Tool* m_tool;
	otoolkit::ToolWidgets m_toolWidgets;
	bool m_isRunning;

	ToolRuntimeHandler() = delete;
};