/*
 *	File:		uiAPI.h
 *	Package:	akAPI
 *
 *  Created on: February 06, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// AK header
#include <akCore/aException.h>
#include <akCore/akCore.h>
#include <akCore/globalDataTypes.h>

#include <akGui/aObjectManager.h>

#include <akWidgets/aComboButtonWidgetItem.h>

// Qt header
#include <qstring.h>					// QString
#include <qicon.h>						// QIcon
#include <qstringlist.h>				// QStringList
#include <qstringlist.h>
#include <qsize.h>
#include <qpixmap.h>
#include <qbytearray.h>

// std header
#include <vector>						// vector<>
#include <map>							// map<>

// Forward declaration
class QSurfaceFormat;
class QWidget;
class QMovie;
class QTabBar;
class QSettings;
class QDockWidget;
class QScreen;
class QApplication;

namespace ak {

	// Forward declaration
	class aMessenger;
	class aNotifier;
	class aUidManager;
	class aObjectManager;
	class aWindowEventHandler;
	
	namespace uiAPI {

		//! The API manager is used to manage the global objects required for this API to work
		class apiManager {
		public:
			//! @brief Constructor
			apiManager();
			
			//! @brief Deconstructor
			virtual ~apiManager();
			
			//! @brief Will initialize the API
			//! @param _messenger The external messenger. If nullptr a new one will be created
			//! @param _uidManager The external UID manager. If nullptr a new one will be created
			//! @param _objectManager The external object manager. If nullptr a new one will be created
			void ini(
				QApplication* _applicationInstance,
				const QString &			_organizationName,
				const QString &			_applicationName
			);

			//! @brief Returns true if the API was initialized
			bool isInitialized(void) const;

			//! @brief Will return the default surface format
			QSurfaceFormat * getDefaultSurfaceFormat(void);

			QApplication* app() { return m_app; }

			QScreen* screen() { return m_screen; }

			QSettings * settings(void) const { return m_settings; }

		protected:
			QApplication*				m_app;							//! The core application
			bool						m_appIsRunning;				//! True if the core application is already running
			QSurfaceFormat *			m_defaultSurfaceFormat;
			QScreen*					m_screen;

			QString						m_companyName;
			QString						m_applicationName;
			QSettings *					m_settings;

			bool						m_isInitialized;				//! If true, then the API was initialized

			aUidManager *				m_fileUidManager;				//! The UID manager used for files in this API
			
		private:
			apiManager(const apiManager &) = delete;
			apiManager & operator = (const apiManager &) = delete;
		};

		// ###############################################################################################################################################

		//! @brief Will initialize the application
		//! @param _argc The program start argument count
		//! @param _argv The program start arguments
		//! @param _organizationName The name of the organization (required for settings set/get)
		//! @param _applicationName The name of the application (required for settings set/get)
		UICORE_API_EXPORT void ini(
			QApplication* _applicationInstance,
			const QString &		_organizationName,
			const QString &		_applicationName
		);

		//! @brief Will destroy all objects created by this API
		UICORE_API_EXPORT void destroy(void);

		//! @brief Will enable the provided event types to be send
		//! @param _types The types to allow
		UICORE_API_EXPORT void enableEventTypes(
			eventType							_types
		);

		//! @brief Will disable the provided event types to be send
		//! @param _types The types to disable
		UICORE_API_EXPORT void disableEventTypes(
			eventType							_types
		);

		//! @brief Will set the enabled state of the messenger
		//! @param _enabled The enabled state to set. If false, no messages will be sent
		UICORE_API_EXPORT void setMessengerEnabled(
			bool								_enabled
		);

		//! @brief Will return the current enabled state for the messenger
		UICORE_API_EXPORT bool messengerIsEnabled(void);

		//! @brief Will return the enabled event types that will be send
		UICORE_API_EXPORT std::vector<eventType> enabledEventTypes(void);

		//! @brief Will return the disabled event types that will be send
		UICORE_API_EXPORT std::vector<eventType> disabledEventTypes(void);

		//! @brief Will return the QWidget of the specifed object
		//! @param _objectUid The UID of the object
		UICORE_API_EXPORT QWidget * getWidget(
			UID									_objectUid
		);

		//! @brief Will return the creator UID of the specified object
		//! @param _objectUID The UID of the object to get its cretor from
		UICORE_API_EXPORT UID getObjectCreator(
			UID									_objectUID
		);

		//! @brief Will return the object manager used in this API
		UICORE_API_EXPORT aObjectManager * getObjectManager(void);

		// ###############################################################################################################################################
		
		// message functions

		//! @brief Will register the notifier at the messenger
		//! The provided notifier will get a new uid if not done so before.
		//! Returns the UID of the notifier
		//! @param _senderUid The sender UID for which to register the provided notifier
		//! @param _notifier The notifier which to register
		//! @throw ak::Exception if the API is not initialized
		UICORE_API_EXPORT void registerUidNotifier(
			UID									_senderUid,
			ak::aNotifier *						_notifier
		);

		//! @brief Will register the notifier at the messenger
		//! The provided notifier will get a new uid if not done so before.
		//! Returns the UID of the notifier
		//! @param _event The event type for which to register the provided notifier
		//! @param _notifier The notifier which to register
		//! @throw ak::Exception if the API is not initialized
		UICORE_API_EXPORT void registerEventTypeNotifier(
			eventType							_event,
			ak::aNotifier *						_notifier
		);

		//! @brief Will register the notifier for all messages send at the global messenger
		//! The provided notifier will get a new uid if not done so before.
		//! Returns the UID of the notifier
		//! @param _notifier The notifier which to register
		//! @throw ak::Exception if the API is not initialized
		UICORE_API_EXPORT void registerAllMessagesNotifier(
			ak::aNotifier *						_notifier
		);

		//! @brief Will send a message to the messanger
		//! @param _senderUid Who sends this message
		//! @param _messageType What type of message is send
		//! @param _message The message itself
		//! @param _info1 Additional information 1
		//! @param _info2 Additional information 2
		//! @throw ak::Exception if the API is not initialized
		//! @throw ak::Exception At any kind of error that may occur in any subroutine call when calling the receivers
		UICORE_API_EXPORT void sendMessage(
			UID									_senderUid,
			eventType							_event,
			int									_info1 = 0,
			int									_info2 = 0
		);

		//! @brief Will set the samples count for the default surface formt
		//! @param _count The count of samples
		UICORE_API_EXPORT void setSurfaceFormatDefaultSamplesCount(
			int									_count
		);

		// ###############################################################################################################################################

		// object creation

		//! @brief Will create a new Action and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		//! @param _text The text of the action
		//! @param _iconSubFolder The sub folder the icon is located at (Folder depends on the search directories)
		//! @param _iconName The name of the icon to display in the action (requires also the icon size to be provided)
		//! @throw ak::Exception if the API is not initialized
		UICORE_API_EXPORT UID createAction(
			UID									_creatorUid,
			const QString &						_text,
			const QString &						_iconName,
			const QString &						_iconSubFolder = QString("32")
		);

		//! @brief Will create a new Action and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		//! @param _text The text of the action
		//! @param _icon The icon of the action
		//! @throw ak::Exception if the API is not initialized
		UICORE_API_EXPORT UID createAction(
			UID									_craetorUid,
			const QString &						_text,
			const QIcon &						_icon
		);

		//! @brief Will create a new CheckBox and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		//! @param _text The initial text of the CheckBox
		//! @param _checked The initial checked state of the checkbox
		//! @throw ak::Exception if the API is not initialized
		UICORE_API_EXPORT UID createCheckbox(
			UID									_creatorUid,
			const QString &						_text,
			bool								_checked = false
		);

		//! @brief Will create a new ComboBox and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		//! @throw ak::Exception if the API is not initialized
		UICORE_API_EXPORT UID createComboBox(
			UID									_creatorUid
		);

		//! @brief Will create a new ComboButton and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		//! @param _text The initial text of the ComboButton
		//! @param _possibleSelection The items the ComboButton will display when showing the drop-down menu
		//! @throw ak::Exception if the API is not initialized
		UICORE_API_EXPORT UID createComboButton(
			UID									_creatorUid,
			const QString &						_text = QString(""),
			const std::vector<QString> &		_possibleSelection = std::vector<QString>()
		);

		//! @brief Will create a line edit and return its UID
		//! param _creatorUid The UID of the creator
		//! @param _initialText The initial text to display
		UICORE_API_EXPORT UID createLineEdit(
			UID									_creatorUid,
			const QString &						_initialText = QString("")
		);

		//! @brief Will create a nice line edit
		//! @param _initialText The initial text of the edit field
		//! @param _infoLabelText The initial text of the information label
		UICORE_API_EXPORT UID createNiceLineEdit(
			UID									_creatorUid,
			const QString &						_initialText,
			const QString &						_infoLabelText
		);

		//! @brief Will create a new PushButton and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		//! @param _text The initial text of the PushButton
		//! @throw ak::Exception if the provided UID is invalid or the API is not initialized
		UICORE_API_EXPORT UID createPushButton(
			UID									_creatorUid,
			const QString &						_text = QString("")
		);

		//! @brief Will create a new PushButton and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		//! @param _icon The icon of the PushButton
		//! @param _text The initial text of the PushButton
		//! @throw ak::Exception if the provided UID is invalid or the API is not initialized
		UICORE_API_EXPORT UID createPushButton(
			UID									_creatorUid,
			const QIcon &						_icon,
			const QString &						_text = QString("")
		);

		//! @brief Will create a new TabToolBar subcountainer and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		//! @param _parentUid The UID of the parent TabToolBar object (may be a uiManager, TabToolBarPage or TabToolBarGroup)
		//! @param _text The text of the SubContainer
		//! @throw ak::Exception if the provided UID is invalid or the API is not initialized
		UICORE_API_EXPORT UID createTabToolBarSubContainer(
			UID									_creatorUid,
			UID									_parentUid,
			const QString &						_text = QString("")
		);

		//! @brief Will create a new timer and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		UICORE_API_EXPORT UID createTimer(
			UID									_creatorUid
		);

		//! @brief Will create a toolButton and return its UID
		//! @param _creatorUid The UID of the creator
		UICORE_API_EXPORT UID createToolButton(
			UID									_creatorUid
		);

		//! @brief Will create a toolButton and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _text The initial text of the toolButton
		UICORE_API_EXPORT UID createToolButton(
			UID									_creatorUid,
			const QString &						_text
		);

		//! @brief Will create a toolButton and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _text The initial text of the toolButton
		//! @param _icon The initial icon of the toolButton
		UICORE_API_EXPORT UID createToolButton(
			UID									_creatorUid,
			const QString &						_text,
			const QIcon &						_icon
		);

		//! @brief Will create a toolButton and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _text The initial text of the toolButton
		//! @param _iconName The name of the initial icon for the toolButton
		//! @param _iconFolder The folder of the initial icon for the toolButton
		UICORE_API_EXPORT UID createToolButton(
			UID									_creatorUid,
			const QString &						_text,
			const QString &						_iconName,
			const QString &						_iconFolder
		);

		//! @brief Will create a new tab view and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		//! @throw ak::Exception if the provided UID is invalid or the API is not initialized
		UICORE_API_EXPORT UID createTabView(
			UID									_creatorUid
		);

		//! @brief Will create a new UI manager and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		//! @throw ak::Exception if the provided UID is invalid or the API is not initialized
		UICORE_API_EXPORT UID createWindow(
			UID									_creatorUid
		);

		// Object creation

		// ###############################################################################################################################################

		// Action

		namespace action {

			//! @brief Will set the enabled state of the action
			//! @param _actionUID The UID of the action
			//! @param _enabled The enabled state to set
			UICORE_API_EXPORT void setEnabled(
				UID								_actionUID,
				bool							_enabled
			);

			//! @brief Will set the text of the action
			//! @param _actionUID The UID of the action
			//! @param _text The text to set
			UICORE_API_EXPORT void setText(
				UID								_actionUID,
				const QString &					_text
			);

			//! @brief Will set the icon of the action
			//! @param _actionUID The UID of the action
			//! @param _icon The icon to set
			UICORE_API_EXPORT void setIcon(
				UID								_actionUID,
				const QIcon &					_icon
			);

			//! @brief Will set the icon of the action
			//! @param _actionUID The UID of the action
			//! @param _iconName The name of the icon
			//! @param _iconFolder The folder the icon is located at
			UICORE_API_EXPORT void setIcon(
				UID								_actionUID,
				const QString &					_iconName,
				const QString &					_iconFolder
			);

			//! @brief Will return the text of the action
			//! @param _actionUID The UID of the action
			UICORE_API_EXPORT QString getText(
				UID								_actionUID
			);

		} // namespace action

		// ###############################################################################################################################################

		// CheckBox

		namespace checkBox {

			UICORE_API_EXPORT void setChecked(
				UID								_checkBoxUID,
				bool							_checked
			);

			UICORE_API_EXPORT void setText(
				UID								_checkBoxUID,
				const QString &					_text
			);

			UICORE_API_EXPORT void setEnabled(
				UID								_checkBoxUID,
				bool							_enabled
			);

			UICORE_API_EXPORT QString text(
				UID								_checkBoxUID
			);

			UICORE_API_EXPORT bool isChecked(
				UID								_checkBoxUID
			);

			UICORE_API_EXPORT bool isEnabled(
				UID								_checkBoxUID
			);

		}

		// ###############################################################################################################################################

		// Container

		namespace container {

			//! @brief Will add an object to the specified container
			//! @param _containerUID The UID of the container
			//! @param _objectUID The UID of the object to add
			UICORE_API_EXPORT void addObject(
				UID								_containerUID,
				UID								_objectUID
			);

			//! @brief Will set the enabled state of the specified container
			//! @param _containerUID The UID of the container
			//! @param _isEnabled The enabled state to set
			UICORE_API_EXPORT void setEnabled(
				UID								_containerUID,
				bool							_isEnabled = true
			);

			//! @brief Will return the enabled state of the specified container
			//! @param _containerUID The UID of the container
			UICORE_API_EXPORT bool isEnabled(
				UID								_containerUID
			);
		}

		// ###############################################################################################################################################

		// Dialog

		namespace dialog {

			//! @brief Will show an open directory dialog and return the selected directory
			UICORE_API_EXPORT QString openDirectory(const QString & _title, const QString & _initialDir);

		}

		// ###############################################################################################################################################

		// Line edit

		namespace lineEdit {
			
			//! @brief Will set the line edit text
			//! @param _lineEditUid The UID of the line edit
			//! @param _text The text to set
			UICORE_API_EXPORT void setText(
				UID													_lineEditUid,
				const QString &										_text
			);

			//! @brief Will set the nice line edit error state
			//! @param _lineEditUid The UID of the line edit
			//! @param _error The error state
			UICORE_API_EXPORT void setErrorState(
				UID													_lineEditUid,
				bool												_error
			);

			//! @brief Will set wather the error state will have an impact on the foreground or the background
			//! @param _lineEditUid The UID of the line edit
			//! @param _isForeground If true, the error state will change the text foreground color, otherwise back and foreground
			UICORE_API_EXPORT void setErrorStateIsForeground(
				UID													_lineEditUid,
				bool												_isForeground
			);

			//! @brief Will set the line edit enabled state
			//! @param _lineEditUid The UID of the line edit
			//! @param _enabled The enabled state to set
			UICORE_API_EXPORT void setEnabled(
				UID													_lineEditUid,
				bool												_enabled
			);

			//! @brief Will return the line edit text
			//! @param _lineEditUid The UID of the line edit
			UICORE_API_EXPORT QString text(
				UID													_lineEditUid
			);

			//! @brief Will return the line edit enabled state
			//! @param _lineEditUid The UID of the line edit
			UICORE_API_EXPORT bool isEnabled(
				UID													_lineEditUid
			);
		}

		// ###############################################################################################################################################

		// Nice line edit

		namespace niceLineEdit {

			//! @brief Will set the nice line edit text
			//! @param _lineEditUid The UID of the line edit
			//! @param _text The text to set
			UICORE_API_EXPORT void setText(
				UID												_lineEditUid,
				const QString &										_text
			);

			//! @brief Will set the nice line edit error state
			//! @param _lineEditUid The UID of the line edit
			//! @param _error The error state
			UICORE_API_EXPORT void setErrorState(
				UID												_lineEditUid,
				bool												_error
			);

			//! @brief Will set wather the error state will have an impact on the foreground or the background
			//! @param _lineEditUid The UID of the line edit
			//! @param _isForeground If true, the error state will change the text foreground color, otherwise back and foreground
			UICORE_API_EXPORT void setErrorStateIsForeground(
				UID													_lineEditUid,
				bool												_isForeground
			);

			//! @brief Will set the nice line edit info label text
			//! @param _lineEditUid The UID of the line edit
			//! @param _text The text to set
			UICORE_API_EXPORT void setInfoLabelText(
				UID												_lineEditUid,
				const QString &										_text
			);

			//! @brief Will set the nice line edit enabled state
			//! @param _lineEditUid The UID of the line edit
			//! @param _enabled The enabled state to set
			UICORE_API_EXPORT void setEnabled(
				UID												_lineEditUid,
				bool												_enabled
			);

			//! @brief Will return the nice line edit text
			//! @param _lineEditUid The UID of the line edit
			UICORE_API_EXPORT QString text(
				UID												_lineEditUid
			);

			//! @brief Will return the nice line edit info label text
			//! @param _lineEditUid The UID of the line edit
			UICORE_API_EXPORT QString infoLabelText(
				UID												_lineEditUid
			);

			//! @brief Will return the nice line edit enabled state
			//! @param _lineEditUid The UID of the line edit
			UICORE_API_EXPORT bool isEnabled(
				UID												_lineEditUid
			);
		}

		// ###############################################################################################################################################

		// Object

		namespace object {

			//! @brief Will destroy the specifed object
			//! @param _objectUID The UID of the object to destroy
			//! @param _ignoreIfObjectHasChildObjects If true, will not destroy the specified object if it still has child objects
			UICORE_API_EXPORT void destroy(
				UID												_objectUID,
				bool												_ignoreIfObjectHasChildObjects = false
			);
			
			//! @brief Will set an object name to the specified object so it can be accessed later.
			//! The name must be unique!
			//! @param _objectUID The UID of the object to set the name at
			//! @param _name The name to set
			UICORE_API_EXPORT void setObjectUniqueName(
				UID												_objectUID,
				const QString &										_name
			);

			//! @brief Will return the unique name set to the object
			//! @param _objectUID The UID of the object
			UICORE_API_EXPORT QString getObjectUniqueName(
				UID												_objectUID
			);

			//! @brief Will return the UID of the object with the specified name
			//! Will return ak::invalidUID if there is no object with the specified name
			//! @param _name The name of the object
			UICORE_API_EXPORT UID getUidFromObjectUniqueName(
				const QString &										_name
			);

			//! @brief Will set the objects ToolTip
			//! For the list of suitable objects refer to the documentation
			//! @param _objectUID The UID of the object
			//! @param _toolTip The toop tip to set
			UICORE_API_EXPORT void setToolTip(
				UID													_objectUID,
				const QString &										_toolTip
			);

			//! @brief Will set the objects enabeld state
			//! For the list of suitable objects refer to the documentation
			//! @param _objectUID The UID of the object
			//! @param _enabled The enabled state to set
			UICORE_API_EXPORT void setEnabled(
				UID													_objectUID,
				bool												_enabled = true
			);

			//! @brief Will return the objects enabeld state
			//! For the list of suitable objects refer to the documentation
			//! @param _objectUID The UID of the object
			UICORE_API_EXPORT bool getIsEnabled(
				UID													_objectUID
			);

			//! @brief Will return true if the specified object exists
			//! @param _objectUID The UID of the object
			UICORE_API_EXPORT bool exists(
				UID													_objectUID
			);

			//! @brief Will return the object type of the specified object
			//! @param _objectUID The UID of the object
			UICORE_API_EXPORT objectType type(
				UID													_objectUID
			);

			//! @brief Will cast and return the object with the specified id
			//! @param _objectUID The UID of the object
			template<class T> T * get(
				UID													_objectUID
			) {
				T * obj = nullptr;
				obj = dynamic_cast<T *>(getObjectManager()->object(_objectUID));
				if (obj == nullptr) { 
					assert(0);
					throw ak::InvalidObjectTypeException(); 
				}
				return obj;
			}

		}

		// ###############################################################################################################################################

		// TabView

		namespace tabWidget {

			UICORE_API_EXPORT ID addTab(
				UID				_tabWidgetUID,
				UID				_widgetUID,
				const QString &		_title
			);

			UICORE_API_EXPORT ID addTab(
				UID				_tabWidgetUID,
				UID				_widgetUID,
				const QString &		_title,
				const QString &		_iconName,
				const QString &		_iconFolder
			);

			UICORE_API_EXPORT ID addTab(
				UID				_tabWidgetUID,
				UID				_widgetUID,
				const QString &		_title,
				const QIcon &		_icon
			);

			UICORE_API_EXPORT ID addTab(
				UID				_tabWidgetUID,
				QWidget *			_widget,
				const QString &		_title
			);

			UICORE_API_EXPORT ID addTab(
				UID				_tabWidgetUID,
				QWidget *			_widget,
				const QString &		_title,
				const QString &		_iconName,
				const QString &		_iconFolder
			);

			UICORE_API_EXPORT ID addTab(
				UID				_tabWidgetUID,
				QWidget *			_widget,
				const QString &		_title,
				const QIcon &		_icon
			);

			//! @brief Will close all tabs of the specified tabWidget
			UICORE_API_EXPORT void closeAllTabs(
				UID				_tabWidgetUID
			);

			UICORE_API_EXPORT void closeTab(
				UID				_tabWidgetUID,
				ID				_tabID
			);

			UICORE_API_EXPORT ID getFocusedTab(
				UID				_tabWidgetUID
			);

			UICORE_API_EXPORT bool getTabsClosable(
				UID				_tabWidgetUID
			);

			UICORE_API_EXPORT QString getTabText(
				UID				_tabWidgetUID,
				ID				_tabID
			);

			UICORE_API_EXPORT void setEnabled(
				UID				_tabWidgetUID,
				bool				_enabled
			);

			UICORE_API_EXPORT void setTabbarLocation(
				UID								_tabWidgetUID,
				tabLocation						_location
			);

			UICORE_API_EXPORT void setTabFocused(
				UID				_tabWidgetUID,
				ID				_tabID
			);

			//! @brief Will set the closeabled state for the tabs in this tabWidget
			//! @param _closeable If true the tabs can be closed by the user
			UICORE_API_EXPORT void setTabsClosable(
				UID								_tabWidgetUID,
				bool								_closeable
			);

			//! @brief Will set the text of the specified tab
			//! @param _tabWidgetUID The UID of the tab view
			//! @param _tab The tab to set the text at
			//! @param _text The text to set
			UICORE_API_EXPORT void setTabText(
				UID								_tabWidgetUID,
				ID								_tab,
				const QString &						_text
			);

			UICORE_API_EXPORT void setVisible(
				UID				_tabWidgetUID,
				bool				_visible
			);

			UICORE_API_EXPORT void setObjectName(
				UID							_tabWidgetUID,
				const QString &					_name
			);

			//! @brief Will return true if a tab with the provided text exists
			//! @param _tabText The text of the tab to look for
			UICORE_API_EXPORT bool hasTab(
				UID							_tabWidgetUID,
				const QString & _tabText
			);

			//! @brief Will return the ID of the first tab with the specified name
			UICORE_API_EXPORT ID getTabIDByText(
				UID							_tabWidgetUID,
				const QString & _tabText
			);

		}

		// ###############################################################################################################################################

		// Timer

		namespace timer {

			UICORE_API_EXPORT int getInterval(
				UID							_timerUID
			);

			UICORE_API_EXPORT void setInterval(
				UID							_timerUID,
				int								_interval
			);

			UICORE_API_EXPORT void shoot(
				UID							_timerUID
			);

			UICORE_API_EXPORT void shoot(
				UID							_timerUID,
				int								_interval
			);

			UICORE_API_EXPORT void start(
				UID							_timerUID
			);

			UICORE_API_EXPORT void start(
				UID							_timerUID,
				int								_interval
			);

			UICORE_API_EXPORT void stop(
				UID							_timerUID
			);

		} // namespace timer

		// ###############################################################################################################################################

		// ToolButton

		namespace toolButton {

			//! @brief Will return the current enabled state of the toolButton
			UICORE_API_EXPORT bool getIsEnabled(
				UID							_toolButtonUID
			);

			//! @brief Will get the text of the toolButton
			//! @param _toolButtonUID The UID of the tool button
			UICORE_API_EXPORT QString getText(
				UID							_toolButtonUID
			);

			//! @brief Will get the current ToolTip of the toolButton
			//! @param _toolButtonUID The UID of the tool button
			UICORE_API_EXPORT QString getToolTip(
				UID							_toolButtonUID
			);

			//! @brief Will set the enabled state of the toolButton
			//! @param _toolButtonUID The UID of the tool button
			//! @param _enabled If true, the toolButton will be enabled
			UICORE_API_EXPORT void setEnabled(
				UID							_toolButtonUID,
				bool							_enabled = true
			);

			//! @brief Will set the text of the toolButton
			//! @param _toolButtonUID The UID of the tool button
			//! @param _text The text to set
			UICORE_API_EXPORT void setText(
				UID							_toolButtonUID,
				const QString &					_text
			);

			//! @brief Will set the ToolTip of the toolButton
			//! @param _toolButtonUID The UID of the tool button
			//! @param _text The text to set
			UICORE_API_EXPORT void setToolTip(
				UID							_toolButtonUID,
				const QString &					_text
			);

			//! @brief Will set the icon of the toolButton
			//! @param _toolButtonUID The UID of the tool button
			//! @param _icon The icon to set
			UICORE_API_EXPORT void setIcon(
				UID							_toolButtonUID,
				const QIcon &					_icon
			);

			//! @brief Will set the icon of the toolButton
			//! @param _toolButtonUID The UID of the tool button
			//! @param _iconName The name of the icon to set
			//! @param _iconFolder The folder of the icon to set
			UICORE_API_EXPORT void setIcon(
				UID							_toolButtonUID,
				const QString &					_iconName,
				const QString &					_iconFolder
			);

		} // namespace toolButton

		// ###############################################################################################################################################

		// Window

		namespace window {

			//! @brief Will add the provided event handler to the window
			//! @param _windowUID The UID of the window
			//! @param _eventHandler The event handler to add
			UICORE_API_EXPORT void addEventHandler(
				UID												_windowUID,
				aWindowEventHandler *								_eventHandler
			);

			//! @brief Will close the provided window
			//! @param _windowUID The UID of the uiManager to close
			UICORE_API_EXPORT void close(
				UID												_windowUID
			);

			//! @brief Will remove the provided event handler from the window
			//! @param _windowUID The UID of the window
			//! @param _eventHandler The event handler to remove
			UICORE_API_EXPORT void removeEventHandler(
				UID												_windowUID,
				aWindowEventHandler *								_eventHandler
			);

			//! @brief Will set the visible state of the status label of the provided window.
			//! @param _windowUID The UID of the window
			//! @param _visible If true the status label will be visible
			UICORE_API_EXPORT void setStatusLabelVisible(
				UID												_windowUID,
				bool												_visible = true,
				bool												_delayed = true
			);

			//! @brief Will set the visible state of the status progress bar of the provided window.
			//! @param _windowUID The UID of the window
			//! @param _visible If true the status progress bar will be visible
			UICORE_API_EXPORT void setStatusProgressVisible(
				UID												_windowUID,
				bool												_visible = true,
				bool												_delayed = true
			);

			//! @brief Will set the status label text at the provided window
			//! @param _windowUID The UID of the window
			//! @param _text The text to set
			UICORE_API_EXPORT void setStatusLabelText(
				UID												_windowUID,
				const QString &										_text
			);

			//! @brief Will set the value of the status progress bar at the provided window
			//! @param _windowUID The UID of the window
			//! @param _value The value to set (0 - 100)
			UICORE_API_EXPORT void setStatusProgressValue(
				UID												_windowUID,
				int													_value
			);

			//! @brief Will set the continuous option of the status progress bar at the provided window
			//! @param _windowUID The UID of the window
			//! @param _continuous If true the status progress bar will be displayed as a continuous bar
			UICORE_API_EXPORT void setStatusProgressContinuous(
				UID												_windowUID,
				bool												_continuous = true
			);

			//! @brief Will set the visible state of the tabToolbar in the specified window
			//! @param _windowUID The UID of the window
			//! @param _visible If true the tabToolbar will be visible
			UICORE_API_EXPORT void setTabToolBarVisible(
				UID												_windowUID,
				bool												_visible = true
			);

			//! @brief Will set the enabled state of double click event in the tab toolbar
			UICORE_API_EXPORT void setTabToolbarDoubleClickEnabled(
				UID												_windowUID,
				bool											_isEnabled
			);

			//! @brief Will return the current tab of the tab tool bar at the specified window
			//! @param _windowUID The UID of the window
			UICORE_API_EXPORT ID getCurrentTabToolBarTab(
				UID												_windowUID
			);

			//! @brief Will return the tab count of the tab tool bar at the specified window
			//! @param _windowUID The UID of the window
			UICORE_API_EXPORT int getTabToolBarTabCount(
				UID												_windowUID
			);

			//! @brief Will enable or disable the tab tool bar at the specified window
			//! @param _windowUID The UID of the window
			//! @param _enable A flag indicating whetehr the toolbar shall be enabled or disabled

			UICORE_API_EXPORT void enableTabToolBar(
				UID												_windowUID,
				bool											_enable
			);

			//! @brief Will set the specified tab toolbar tab as current at the specified window
			//! @param _windowUID The UID of the window
			//! @param _tabID The tab to set as current
			UICORE_API_EXPORT void setCurrentTabToolBarTab(
				UID												_windowUID,
				ID												_tabID
			);

			UICORE_API_EXPORT void setCurrentTabToolBarTab(
				UID												_windowUID,
				const QString&									_tabName
			);

			//! @brief Will set the title at the specified window
			//! @param _windowUID The UID of the window
			//! @param _title The title to set
			UICORE_API_EXPORT void setTitle(
				UID												_windowUID,
				const QString &										_title
			);

			//! @brief Will set the icon at the specified window
			//! @param _windowUID The UID of the window
			//! @param _icon The icon to set
			UICORE_API_EXPORT void setWindowIcon(
				UID												_windowUID,
				const QIcon &									_icon
			);

			//! @brief Will return true if the status label is visible at the provided window
			//! @param _windowUID The UID of the window
			UICORE_API_EXPORT bool getStatusLabelVisible(
				UID												_windowUID
			);

			//! @brief Will return true if the status progress bar is visible at the provided window
			//! @param _windowUID The UID of the window
			UICORE_API_EXPORT bool getStatusProgressVisible(
				UID												_windowUID
			);

			//! @brief Will return the status label text at the provided window
			//! @param _windowUID The UID of the window
			UICORE_API_EXPORT QString getStatusLabelText(
				UID												_windowUID
			);

			//! @brief Will return the status progress bar value at the provided window
			//! @param _windowUID The UID of the window
			UICORE_API_EXPORT int getStatusProgressValue(
				UID												_windowUID
			);

			//! @brief Will return the continuous option of the status progress bar at the provided window
			//! @param _windowUID The UID of the window
			UICORE_API_EXPORT bool getStatusProgressContinuous(
				UID												_windowUID
			);

			//! @brief Will set the windows central widget (The widget will be added to a container, the actual central widget differs)
			//! @param _windowUID The UID of the window
			//! @param _widgetUID The widget UID of the widget to set as central widget
			UICORE_API_EXPORT void setCentralWidget(
				UID												_windowUID,
				UID												_widgetUID
			);

			//! @brief Will set the windows central widget (The widget will be added to a container, the actual central widget differs)
			//! @param _windowUID The UID of the window
			//! @param _widget The widget to set as central widget
			UICORE_API_EXPORT void setCentralWidget(
				UID												_windowUID,
				QWidget *											_widget
			);

			//! @brief Will show the window as maximized
			//! @param _windowUID The UID of the window
			UICORE_API_EXPORT void showMaximized(
				UID												_windowUID
			);

			//! @brief Will show the window
			//! @param _windowUID The UID of the window
			UICORE_API_EXPORT void showNormal(
				UID												_windowUID
			);

			//! @brief Will show the window as minimized
			//! @param _windowUID The UID of the window
			UICORE_API_EXPORT void showMinimized(
				UID												_windowUID
			);

			//! @brief Will return the size of the window
			//! @param _windowUID The UID of the window
			UICORE_API_EXPORT QSize size(
				UID												_windowUID
			);

			//! @brief Will return the position of the window
			//! @param _windowUID The UID of the window
			UICORE_API_EXPORT QPoint position(
				UID												_windowUID
			);

			//! @brief Will set the visible state of the waiting animation
			//! @param _windowUID The UID of the window
			//! @param _visible The visible state to set
			UICORE_API_EXPORT void setWaitingAnimationVisible(
				UID												_windowUID,
				bool												_visible = true
			);

			//! @brief Will set the movie as waiting animation in the window
			//! @param _windowUID The UID of the window
			//! @param _movie The movie to set
			UICORE_API_EXPORT void setWaitingAnimation(
				UID												_windowUID,
				QMovie *											_movie
			);

			//! @brief Will set the animation as waiting animation in the window
			//! @param _windowUID The UID of the window
			//! @param _animationName The name of the animation
			UICORE_API_EXPORT void setWaitingAnimation(
				UID												_windowUID,
				const QString &										_animationName
			);

			//! @brief Will set the central widget minimum size
			//! @param _windowUID The UID of the window
			//! @param _width The width to set
			//! @param _height The height to set
			UICORE_API_EXPORT void setCentralWidgetMinimumSize(
				UID												_windowUID,
				int													_width,
				int													_height
			);

			//! @brief Will return the device pixel ratio of the currently used device
			UICORE_API_EXPORT int devicePixelRatio(void);

			//! @brief Will resize the window
			UICORE_API_EXPORT void resize(
				UID												_windowUID,
				int													_width,
				int													_height
			);

			//! @brief Will return the state of the window so it can be restored
			UICORE_API_EXPORT std::string saveState(
				UID												_windowUID,
				std::string										_currentState = ""
			);

			//! @brief Will return the state of the window so it can be restored
			UICORE_API_EXPORT bool restoreState(
				UID												_windowUID,
				const std::string &								_state,
				bool											_setPositionAndSize = true
			);

			UICORE_API_EXPORT void setTabToolBarTabOrder(
				UID												_windowUID,
				const QStringList&								_list
			);

			UICORE_API_EXPORT void activateToolBarTab(
				UID												_windowUID,
				const QString&								    _tabName
			);

		} // namespace window

		// ###############################################################################################################################################

		// Crypto

		namespace crypto {

			UICORE_API_EXPORT QString hash(
				const QString &			_orig,
				HashAlgorithm			_algorithm
			);

		}

		// ###############################################################################################################################################

		//! @brief Will destroy all objects created by this creator
		//! @param _creatorUid The UID of the creator
		//! @throw ak::Exception if the API is not initialized
		UICORE_API_EXPORT void creatorDestroyed(
			UID									_creatorUid
		);

		// ###############################################################################################################################################

		// parameters

		//! @brief Will generate a new UID and return it
		//! @throw ak::Exception if the API is not initialized
		UICORE_API_EXPORT UID createUid(void);

		//! @brief Will return the messenger
		//! @throw ak::Exception if the API is not initialized
		UICORE_API_EXPORT aMessenger * getMessenger(void);

		//! @brief WIll return the icon with the specified name and path
		//! @throw ak::Exception if the API is not initialized
		UICORE_API_EXPORT const QIcon & getIcon(
			const QString &											_iconName,
			const QString &											_iconSubPath
		);

		//! @brief Will return the application icon with the specified name and path
		UICORE_API_EXPORT const QIcon & getApplicationIcon(
			const QString &											_iconName
		);

		//! @brief Will return the icon with the specified name and path
		UICORE_API_EXPORT const QPixmap & getPixmap(
			const QString &											_name
		);

		//! @brief Will return the movie with the specified name and path
		//! @param _name The name of the movie
		UICORE_API_EXPORT QMovie * getMovie(
			const QString&											_name
		);

		// ###############################################################################################################################################

		// Settings

		namespace settings {

			UICORE_API_EXPORT QString getString(
				const QString &			_settingsName,
				const QString &			_defaultValue = QString("")
			);

			UICORE_API_EXPORT int getInt(
				const QString &			_settingsName,
				int						_defaultValue = 0
			);

			UICORE_API_EXPORT double getDouble(
				const QString &			_settingsName,
				double					_defaultValue = 0.0
			);

			UICORE_API_EXPORT float getFloat(
				const QString &			_settingsName,
				float					_defaultValue = 0.0
			);

			UICORE_API_EXPORT bool getBool(
				const QString &			_settingsName,
				bool					_defaultValue = 0.0
			);

			UICORE_API_EXPORT QByteArray getByteArray(
				const QString &			_settingsName,
				const QByteArray &		_defaultValue = QByteArray()
			);

			UICORE_API_EXPORT void setString(
				const QString &			_settingsName,
				const QString &			_value
			);

			UICORE_API_EXPORT void setInt(
				const QString &			_settingsName,
				int						_value
			);

			UICORE_API_EXPORT void setDouble(
				const QString &			_settingsName,
				double					_value
			);

			UICORE_API_EXPORT void setFloat(
				const QString &			_settingsName,
				float					_value
			);

			UICORE_API_EXPORT void setBool(
				const QString &			_settingsName,
				bool					_value
			);

			UICORE_API_EXPORT void setByteArray(
				const QString &			_settingsName,
				const QByteArray &		_value
			);
		}

		// ###############################################################################################################################################

		// special

		namespace special {

			//! @brief Will return a string representation of the provided eventType
			//! @param _type The event type that should be represented
			UICORE_API_EXPORT QString toString(
				eventType								_type
			);

			//! @brief Will return a string representation of the provided valueType
			//! @param _type The value type that should be represented
			UICORE_API_EXPORT QString toString(
				valueType								_type
			);

			//! @brief Will return a string representation of the provided valtextAlignmentueType
			//! @param _type The text alignment that should be represented
			UICORE_API_EXPORT QString toString(
				textAlignment							_type
			);

			//! @brief Will return a string representation of the provided dockLocation
			//! @param _type The dock location that should be represented
			UICORE_API_EXPORT QString toString(
				dockLocation							_dockLocation
			);

			//! @brief Will return a string representation of the provided tab location
			//! @param _type The tab location that should be represented
			UICORE_API_EXPORT QString toString(
				tabLocation								_tabLocation
			);

			//! @brief Will return a string representation of the provided keyType
			//! @param _type The key type that should be represented
			UICORE_API_EXPORT QString toString(
				keyType									_type
			);

			//! @brief Will return a string representation of the provided objectType
			//! @param _type The object type that should be represented
			UICORE_API_EXPORT QString toString(
				objectType								_type
			);
			
			UICORE_API_EXPORT QString toString(
				const QDate &							_date,
				dateFormat								_format,
				const QString &							_delimiter
			);

			UICORE_API_EXPORT QString toString(
				const QTime &							_time,
				timeFormat								_format,
				const QString &							_delimiter,
				const QString &							_millisecondDelimiter = "."
			);

			//! @brief Will create a JSON type representation of the event
			UICORE_API_EXPORT QString toEventText(
				UID										_senderUID,
				eventType								_eventType,
				int										_info1,
				int										_info2
			);

		}

	} // namespace uiAPI

} // namespace ak