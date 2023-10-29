//! @file Tool.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API
#include "OToolkitAPI/otoolkitapi_global.h"

// OpenTwin header
#include "OpenTwinCore/OTClassHelper.h"
#include "OTWidgets/TabWidget.h"

// Qt header
#include <QtCore/qlist.h>
#include <QtGui/qicon.h>
#include <QtWidgets/qmenu.h>

namespace otoolkit {

	class Tool {
		OT_DECL_NOCOPY(Tool)
	public:
		Tool() {};
		virtual ~Tool() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Pure virtual functions

		//! @brief Return the unique tool name
		//! The name will be used to create all required menu entries
		virtual QString toolName(void) const = 0;

		//! @brief Create the tool menu that will be displayed in the MenuBar->Tools->ToolName
		//! The menu already contains the following items:
		//!   Run / Stop (Starts or stops the tool)
		//!   [] Autorun (Autostart mode checkbox for the tool upon OToolkit start)
		virtual void createToolMenu(QMenu * _root) = 0;

		//! @brief Create the central widget that will be displayed to the user in the main tab view
		virtual ot::TabWidget* createCentralToolWidget(void) = 0;

		//! @brief Create the widgets that will be displayed in the status bar (if tool has focus)
		virtual QList<QWidget*> createToolStatusWidgets(void) = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Optional virtual functions

		//! @brief Return the icon that will be used in the tool menu
		virtual QIcon toolIcon(void) const { return QIcon(); };

		//! @brief Stop all the logic of this tool
		virtual bool prepareToolShutdown(void) {};

		virtual void toolWasShown(void) {};

		virtual void toolWasHidden(void) {};
	};

}