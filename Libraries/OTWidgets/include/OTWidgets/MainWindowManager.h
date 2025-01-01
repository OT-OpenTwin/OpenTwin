//! @file MainWindowManager.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/OTClassHelper.h"

namespace ot {

	class MainWindow;
	class AbstractToolBar;
	class AbstractStatusBar;
	class CentralWidgetManager;

	class MainWindowManager {
		OT_DECL_NOCOPY(MainWindowManager)
		OT_DECL_NODEFAULT(MainWindowManager)
	public:
		enum class CreationFlag {
			NoFlags               = 0 << 0, //! \brief Create only window. No managers are created.

			CreateCentralWidget   = 1 << 0, //! \brief Central widget manager will be created and assigned.
			//! \brief Setup default central widget manager.
			//! \note Has no effect if CreationFlag::CreateCentralWidget is not set.
			SetupCentralWidget    = 1 << 1,

			CreateStatusBar       = 1 << 2, //! \brief Status bar manager will be created and assigned.
			//! \brief Setup default status bar manager.
			//! \note Has no effect if CreationFlag::CreateStatusBar is not set.
			SetupStatusBar        = 1 << 3,

			//! \brief Create default OpenTwin window without any tool bar.
			DefaultWithoutToolBar = CreateCentralWidget | SetupCentralWidget | CreateStatusBar | SetupStatusBar,

			CreateTabToolBar      = 1 << 4, //! \brief Tool bar manager will be created and assigned.
			//! \brief Setup default tool bar manager.
			//! \note Has no effect if CreationFlag::CreateTabToolBar is not set.
			SetupTabToolBar       = 1 << 5,
			
			//! \brief Create default OpenTwin window with the tab tool bar.
			DefaultWithTabToolBar = DefaultWithoutToolBar | CreateTabToolBar | SetupTabToolBar
		};
		typedef Flags<CreationFlag> CreationFlags;

		MainWindowManager(const CreationFlags& _flags);
		virtual ~MainWindowManager();

		//! @brief Sets the provided CentralWidgetManager as current.
		//! Destroys any existing CentralWidgetManager.
		//! @param _newManager Manager to set.
		void setCentralWidgetManager(CentralWidgetManager* _newManager);

		//! @brief Returns the current CentralWidgetManager.
		//! MainWindowManager keeps the ownership.
		CentralWidgetManager* getCentralWidgetManager(void) { return m_centralWidgetManager; };
		const CentralWidgetManager* getCentralWidgetManager(void) const { return m_centralWidgetManager; };

		//! @brief Sets the provided ToolBar as current.
		//! Destroys any existing AbstractToolBar.
		//! @param _newToolBar ToolBar to set as current. May be null.
		void setToolBar(AbstractToolBar* _newToolBar);

		//! @brief Returns the current ToolBar.
		//! MainWindowManager keeps the ownership.
		AbstractToolBar* getToolBar(void) { return m_toolBar; };

		//! @brief Returns the current ToolBar.
		//! MainWindowManager keeps the ownership.
		const AbstractToolBar* getToolBar(void) const { return m_toolBar; };

		//! @brief Sets the provided AbstractStatusBar as current.
		//! Destroys any existing AbstractStatusBar.
		//! @param _newStatusBar StatusBar to set.
		void setStatusBar(AbstractStatusBar* _newStatusBar);
		AbstractStatusBar* getStatusBar(void) { return m_statusBar; };
		const AbstractStatusBar* getStatusBar(void) const { return m_statusBar; };

		MainWindow* getWindow(void) { return m_window; };
		const MainWindow* getWindow(void) const { return m_window; };

	private:
		MainWindow* m_window;
		CentralWidgetManager* m_centralWidgetManager;
		AbstractToolBar* m_toolBar;
		AbstractStatusBar* m_statusBar;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::MainWindowManager::CreationFlag)