//! @file ToolManager.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkitAPI header
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OpenTwinCore/OTClassHelper.h"

// Qt header
#include <QtWidgets/qmenu.h>

// std header
#include <list>

class ToolManager {
	OT_DECL_NOCOPY(ToolManager)
public:
	static ToolManager& instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	//! @brief Stores the provided tool
	void addTool(otoolkit::Tool* _tool);

	//! @brief Returns the tool with the specified name
	//! If no tool was found nullptr will be returned
	otoolkit::Tool* findTool(const QString& _toolName);

	//! @brief Cleans up and deletes the tool with the provided name
	void removeTool(const QString& _toolName);

	//! @brief Cleans up the provided tool
	//! The tool will not be deleted
	void removeTool(otoolkit::Tool* _tool);

	void clear(void);

private:
	std::list<otoolkit::Tool *> m_tools;

	ToolManager();
	virtual ~ToolManager();
};