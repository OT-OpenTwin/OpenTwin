#pragma once

// Toolkit API
#include "OToolkitAPITypes.h"
#include "ToolManager.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtGui/qicon.h>
#include <QtWidgets/qwidget.h>

class QMenuBar;

namespace OToolkitAPI {

	class AbstractTool {
	public:
		AbstractTool() {};
		virtual ~AbstractTool() {};

		OToolkitAPI::ToolID toolId(void) const { return m_toolId; };

		// ##########################################################################################################################

		//! @brief Returns the unique name of this tool
		//! @note If another tool with the same name was registered before, the instance of this tool will be destroyed
		virtual QString toolName(void) const = 0;

		//! @brief Returns the icon that represents this tool
		//! This icon is used as icon in the toolbar
		virtual QIcon toolIcon(void) { return QIcon(); };

		//! @brief Returns the widgets that should be placed in the toolbar
		//! The toolbar does NOT take the ownership
		virtual QList<QWidget *> statusBarWidgets(void) const { return QList<QWidget *>(); };

		//! @brief Create all required menu bar entries
		virtual void createMenuBarEntries(QMenuBar * _menuBar) {};

		//! @brief Is called after the user switched to this tool
		virtual void toolWasShown(void) {};

		//! @brief Is called after the user switched to a different tool
		virtual void toolWasHidden(void) {};

		// ##########################################################################################################################

		// Helper functions

		//! @brief Will return the application name
		//! Primarly used to initialize the application settings
		QString applicationName(void) const;

		//! @brief Will display the provided text in the status bar
		//! May be called from a different thread than the main thread
		void displayStatusText(const QString& _text) const;

		//! @brief Will display the provided text as an error message in the status bar
		//! May be called from a different thread than the main thread
		void displayErrorStatusText(const QString& _text) const;

	private:
		friend class OToolkitAPI::ToolManager;

		void setToolId(OToolkitAPI::ToolID _id) { m_toolId = _id; };

		OToolkitAPI::ToolID	m_toolId;

	};
}