// @otlicense

//! @file Tool.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API
#include "OToolkitAPI/ToolWidgets.h"
#include "OToolkitAPI/otoolkitapi_global.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/WidgetView.h"

// Qt header
#include <QtCore/qlist.h>
#include <QtCore/qstring.h>
#include <QtCore/qsettings.h>
#include <QtGui/qicon.h>
#include <QtWidgets/qmenu.h>

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
		virtual QString getToolName(void) const = 0;

		//! @brief Create all widgets.
		//! The menu already contains the following items:
		//!   Run / Stop (Starts or stops the tool)
		//!   [] Autorun (Autostart mode checkbox for the tool upon OToolkit start)
		//! @param _rootMenu Menu where the menu controls should be added to.
		//! @param _content The tool contents must be set here
		virtual bool runTool(QMenu* _rootMenu, ToolWidgets& _content) = 0;

		virtual void restoreToolSettings(QSettings& _settings) {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Optional virtual functions

		//! @brief Return the icon that will be used in the tool menu
		virtual QIcon getToolIcon(void) const { return QIcon(); };

		//! @brief Stop all the logic of this tool
		virtual bool prepareToolShutdown(QSettings& _settings) { return true; };

		virtual void toolWasShown(void) {};

		virtual void toolWasHidden(void) {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setToolIsRunning(bool _isRunning) { m_isRunning = _isRunning; };
		bool getToolIsRunning(void) const { return m_isRunning; }

		void setToolIsExternal(bool _isExternal) { m_isExternal = _isExternal; };
		bool getToolIsExternal(void) const { return m_isExternal; };

	protected:
		ot::WidgetView* createCentralWidgetView(QWidget* _widget, const QString& _widgetName) const;
		ot::WidgetView* createSideWidgetView(QWidget* _widget, const QString& _widgetName) const;
		ot::WidgetView* createToolWidgetView(QWidget* _widget, const QString& _widgetName) const;

	private:
		ot::WidgetView* createWidgetView(QWidget* _widget, const QString& _widgetName) const;

		bool m_isRunning;
		bool m_isExternal;
	};

}