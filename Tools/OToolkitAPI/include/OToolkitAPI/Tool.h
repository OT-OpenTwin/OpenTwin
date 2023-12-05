//! @file Tool.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API
#include "OToolkitAPI/otoolkitapi_global.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qlist.h>
#include <QtCore/qstring.h>
#include <QtCore/qsettings.h>
#include <QtGui/qicon.h>
#include <QtWidgets/qmenu.h>

// std header
#include <list>

class QWidget;

namespace otoolkit {

	class OTOOLKITAPI_EXPORT Tool {
		OT_DECL_NOCOPY(Tool)
	public:
		Tool();
		virtual ~Tool();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Pure virtual functions

		//! @brief Return the unique tool name
		//! The name will be used to create all required menu entries
		virtual QString toolName(void) const = 0;

		//! @brief Create the central widget that will be displayed to the user in the main tab view and the tool menu
		//! //! The menu already contains the following items:
		//!   Run / Stop (Starts or stops the tool)
		//!   [] Autorun (Autostart mode checkbox for the tool upon OToolkit start)
		virtual QWidget* runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets, QSettings& _settings) = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Optional virtual functions

		//! @brief Return the icon that will be used in the tool menu
		virtual QIcon toolIcon(void) const { return QIcon(); };

		//! @brief Stop all the logic of this tool
		virtual bool prepareToolShutdown(QSettings& _settings) { return true; };

		virtual void toolWasShown(void) {};

		virtual void toolWasHidden(void) {};
	};

}