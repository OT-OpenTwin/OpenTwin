#pragma once

// Toolkit API
#include "OToolkitAPITypes.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <map>

namespace OToolkitAPI {

	class AbstractTool;

	class ToolManager {
	public:
		static ToolManager& instance(void);

		//! @brief Will add the provided tool to the storage
		ToolID add(AbstractTool * _tool);

		AbstractTool * getTool(ToolID _id);

		AbstractTool * getToolByName(const QString& _name);

		void unloadTool(ToolID _id);

		void removeToolFromList(ToolID _id);

	private:
		ToolID								m_currentId;
		std::map<ToolID, AbstractTool *>	m_tools;

		ToolManager();
		virtual ~ToolManager();
	};

}