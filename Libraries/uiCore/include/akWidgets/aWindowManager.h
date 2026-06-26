// @otlicense
// File: aWindowManager.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// C++ header
#include <map>

// Qt header
#include <qobject.h>
#include <qstring.h>					// QString
#include <qbytearray.h>
#include <qsize.h>
#include <qstringlist.h>

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akCore/aObject.h>

// Forward declaration
class QWidget;
class QDockWidget;
class QLabel;
class QProgressBar;
class QTimer;
class QMovie;
namespace tt { class TabToolbar; }

namespace ak {

	// Forward declaration
	class aMessenger;
	class aObjectManager;
	class aTtbContainer;
	class aTtbPage;
	class aUidManager;
	class aWindow;
	class aWidget;
	class aWindowEventHandler;
	class aWindowManagerTimerSignalLinker;

	//! @brief This class is used to manage a QMainWindow
	//! It provides several functions to create and manipulate the UI
	//! Also it will connect all created objects to the messaging system and will manage the UIDs of those objects.
	class UICORE_API_EXPORT aWindowManager : public QObject, public aObject {
		Q_OBJECT
	public:
		// #############################################################################################################

		// Constructors

		//! @brief Constructor
		//! @param _messenger The global messenger used in this object
		//! @param _uidManager The global UID manager used in this object
		aWindowManager(
			aMessenger *									_messenger,
			aUidManager *									_uidManager
		);

		//! @brief Deconstructor
		virtual ~aWindowManager();

		//! @brief Will remove the child from this object (not destroy it)
			//! This function should be called from the deconstructor of a child
		virtual void removeChildObject(
			aObject *								_child
		) override;

		//! @brief Will set the alias for this object
		//! @param _alias The alias to set
		void SetObjectName(
			const QString &							_alias
		);

		// #############################################################################################################
		// #############################################################################################################
		// #############################################################################################################
		// ELEMENT CREATION

		void setCentralWidget(
			QWidget *												_centralWidget
		);

		void setWaitingAnimationVisible(
			bool									_visible
		);

		void setWaitingAnimation(
			QMovie *							_movie
		);

		// #############################################################################################################

		// TabToolbar

		//! @brief Will set the visible status of the tabToolbar
		//! @param _vis If true, the tab toolBar will be visible
		void setTabToolBarVisible(
			bool						_vis = true
		);

		//! @brief Will set the enabled state of the double click event for the tab toolbar tabs
		void setTabToolbarDoubleClickEnabled(
			bool						_isEnabled
		);

		//! @brief Will add a new sub container to the tab toolbar container
		//! @param _text The initial text of the container
		aTtbPage * createTabToolbarSubContainer(
			const QString &				_text = QString("")
		);

		//! @brief Will return the sub container with the specified text
		//! Returns nullptr if no sub container with the specified text exists
		//! @param _text The text of the sub container to find
		aTtbContainer * getTabToolBarSubContainer(
			const QString &				_text
		);

		QString getTabToolBarTabText(ak::UID _tabId);

		//! @brief Will add the object with the provided uid to the specified parent object
		//! The object must have been creted by the objectManager used in this tab toolbar manager.
		//! Valid objects are actions or any object derived from aWidget
		//! @param _objectUid The UID of the object to be added
		//! @param _parentUid The UID of the parent object where to add the Object to

		void addTabToolbarWidget(
			UID						_parentUid,
			UID						_objectUid
		);

		//! @brief Will return the current selected tab toolbar tab
		UID currentTabToolbarTab(void) const;

		//! @brief Will return the ammount of tabs in the tabToolbar
		int tabToolbarTabCount(void) const;

		//! @brief Will enable or disable the tabToolbar
		void enableTabToolbar(bool flag) const;

		//! @brief Will set the specified tab toolbar tab as current
		//! @param _tabUID The tab to set as current
		void setCurrentTabToolBarTab(
			UID						_tabUID
		);

		void setCurrentTabToolBarTab(
			const QString&			_tabName
		);

		//! @brief Will set the minimum size of the central widget
		//! @param _size The size to set as minimum size
		void setCentralWidgetMinimumSize(
			const QSize &				_size
		);

		// #############################################################################################################

		// Window

		//! @brief Returns a pointer to the QMainWindow this UI Manager is managing
		aWindow * window(void) const;

		//! @brief Will est the window title to the title provided
		//! @param _title The title to set
		void setWindowTitle(
			const QString &											_title
		);

		//! @brief Will set the window icon to the icon provided
		//! @param _icon The icon to set
		void setWindowIcon(
			const QIcon &											_tcon
		);

		//! @brief Will return the currently set window title
		QString windowTitle(void) const;

		//! @brief Will add the provided event handler to this window
		//! @param _eventHandler The event handler to add
		void addEventHandler(
			aWindowEventHandler *					_eventHandler
		);

		//! @brief Will remove the provided event handler from this window
		//! @param _eventHandler The event handler to remove
		void removeEventHandler(
			aWindowEventHandler *					_eventHandler
		);

		// #############################################################################################################
		// #############################################################################################################
		// #############################################################################################################

		// ELEMENT MANIPULATION

		//! @brief Will set the window state to maximized
		void showMaximized(void);

		//! @brief Will set the window state to minimized
		void showMinimized(void);

		//! @brief Will set the window state to normal
		void showNormal(void);

		//! @brief Will close the window
		void close(void);

		void resize(
			int		_width,
			int		_height
		);

		//! @brief Will return the state of the window so it can be restored
		std::string saveState(std::string currentState);

		bool restoreState(
			const std::string &								_state,
			bool											_setPositionAndSize = true
		);
		
		void setTabToolBarTabOrder(const QStringList& _list);

	Q_SIGNALS:
		void tabToolBarTabChanged(int _index);

	private Q_SLOTS:
		void slotRestoreSetting(const QByteArray & _actualState);
		void slotTabToolbarTabClicked(int _index);
		void slotTabToolbarTabCurrentTabChanged(int _index);

	private:
		// Variables

		void resortTabToolBarTabOrder(void);

		aWindow *							m_window;						//! The QMainWindow the UI manager is applying its changes to

		aMessenger *						m_messenger;					//! The messenger used in this object
		aUidManager *						m_uidManager;					//! The UID manager used in this object
		tt::TabToolbar *					m_tabToolBar;					//! The TabToolBar of this window
		aWindowManagerTimerSignalLinker *	m_timerSignalLinker;			//! The signal linker used to link the timer singnals

		QStringList							m_tabToolBarTabOrder;
		
		QTimer *							m_timerShowMainWindow;			//! The timer used to show the main window upon creation
		std::vector<aTtbContainer *>		m_tabToolBarContainer;
	};
} // namespace ak
