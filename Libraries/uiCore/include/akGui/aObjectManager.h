/*
 *	File:		aObjectManager.h
 *	Package:	akGui
 *
 *  Created on: July 14, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akGui/aColor.h>

// C++ header
#include <map>							// map
#include <exception>

// Qt header
#include <qstring.h>
#include <qicon.h>
#include <qtoolbutton.h>				// QToolButton popup mode
#include <qpixmap.h>

namespace ak {

	class aObject;
	class aNotifierObjectManager;
	class aMessenger;
	class aPaintable;
	class aUidManager;
	class aSignalLinker;
	class aColorStyle;
	class aGlobalKeyListener;
	class aToolButtonWidget;

	//! @brief The objectManager is used for creating and manipulating objects and widgets
	//! All objects created here a derived from ak::core::aObject or child
	class UICORE_API_EXPORT aObjectManager {
	public:
		//! @brief Constructor
		//! @param _messenger The globally used messaging system
		//! @param _uidManager The globally used uidManager
		//! @param _colorStyle The globally used color style
		aObjectManager(
			aMessenger *				_messenger,
			aUidManager *				_uidManager
		);

		//! @brief Deconstructor
		virtual ~aObjectManager();

		// ###############################################################################################################################################

		// Object creation

		//! @brief Will create a action and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _text The initial text of the action
		//! @param _icon The initial icon of the action
		//! @param _popupMode The popup mode of the action
		UID createAction(
			UID												_creatorUid,
			const QString &										_text,
			const QIcon &										_icon,
			QToolButton::ToolButtonPopupMode					_popupMode = QToolButton::ToolButtonPopupMode::InstantPopup
		);

		//! @brief Will create a check box and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _text The initial text of the check box
		UID createCheckBox(
			UID												_creatorUid,
			const QString &										_text = QString(""),
			bool												_checked = false
		);

		//! @brief Will create a color edit button and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _color The initial color set to the color edit button
		//! @param _textOverride If a text is provided then the default color text will be overridden
		UID createColorEditButton(
			UID												_creatorUid,
			const aColor &								_color,
			const QString &										_textOverride = QString("")
		);

		UID createColorStyleSwitch(
			UID						_creatorUid,
			const QString &			_brightModeTitle,
			const QString &			_darkModeTitle,
			const QIcon &			_brightModeIcon,
			const QIcon &			_darkModeIcon,
			bool					_isBright
		);

		//! @brief Will create a combo box and return its UID
		//! @param _creatorUid The UID of the creator
		UID createComboBox(
			UID												_creatorUid
		);

		//! @brief Will create a combo button and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _initialText The initial text of the combo button
		//! @param _possibleSelection The items that can be selected in the combo button
		UID createComboButton(
			UID												_creatorUid,
			const QString &										_initialText = QString(""),
			const std::vector<QString> &						_possibleSelection = std::vector<QString>()
		);

		//! @brief Will create a combo button item and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _icon The icon of the combo button item
		UID createComboButtonItem(
			UID												_creatorUid,
			const QIcon &										_icon,
			const QString &										_text = QString("")
		);

		//! @brief Will create a dock and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _text The text of the dock
		UID createDock(
			UID												_creatorUid,
			const QString &										_text = QString()
		);

		//! @brief Will create a new DockWatcher and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		//! @param _text The initial text of the DockWatcher
		UID createDockWatcher(
			UID									_creatorUid,
			const QString &						_text = QString("Docks")
		);

		//! @brief Will create a new DockWatcher and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		//! @param _icon The initial icon of the DockWatcher
		//! @param _text The initial text of the DockWatcher
		UID createDockWatcher(
			UID									_creatorUid,
			const QIcon &						_icon,
			const QString &						_text = QString("Docks")
		);

		UID createGlobalKeyListener(
			UID											_creatorUid,
			QApplication *								_app,
			Qt::Key										_key,
			const std::vector<Qt::KeyboardModifier>&	_keyModifier,
			bool										_blockOthers
		);

		//! @brief Will create a line edit and return its UID
		//! param _creatorUid The UID of the creator
		//! @param _initialText The initial text to display
		UID createLineEdit(
			UID													_creatorUid,
			const QString &											_initialText = QString("")
		);

		//! @brief Will create a log in dialog and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _showSavePassword if true the save password checkbox will be displayed
		//! @param _username The initial username
		//! @param _password The initial password
		UID createLogInDialog(
			UID												_creatorUid,
			bool												_showSavePassword,
			const QPixmap &										_backgroundImage,
			const QString &										_username = QString(),
			const QString &										_password = QString()
		);

		//! @brief Will create a nice line edit
		//! @param _initialText The initial text of the edit field
		//! @param _infoLabelText The initial text of the information label
		UID createNiceLineEdit(
			UID												_creatorUid,
			const QString &										_initialText,
			const QString &										_infoLabelText
		);

		//! @brief Will create a push button and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _text The text of the push button
		UID createPushButton(
			UID												_creatorUid,
			const QString &										_text = QString("")
		);

		//! @brief Will create a push button and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _icon The icon of the push button
		//! @param _text The text of the push button
		UID createPushButton(
			UID												_creatorUid,
			const QIcon &										_icon,
			const QString &										_text = QString("")
		);

		//! @brief Will create a property grid and return its UID
		//! @param _creatorUid The UID of the creator
		UID createPropertyGrid(
			UID												_creatorUid
		);

		//! @brief Will create a new options dialog and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		UID createOptionsDialog(
			UID												_creatorUid,
			const QString &										_title
		);

		//! @brief Will create a new special TabBar and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		UID createSpecialTabBar(
			UID												_creatorUid
		);

		//! @brief Will create a table and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _rows The initial row count of the table
		//! @param _columns The initial column count of the table
		UID createTable(
			UID												_creatorUid,
			int													_rows,
			int													_columns
		);

		//! @brief Will create a text edit and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _initialText The initial text of the text edit
		UID createTextEdit(
			UID												_creatorUid,
			const QString &										_initialText = QString("")
		);

		//! @brief WIll create a timer and return its UID
		//! @param _creatorUid The UID of the creator
		UID createTimer(
			UID												_creatorUid
		);

		//! @brief Will create a toolButton and return its UID
		//! @param _creatorUid The UID of the creator
		UID createToolButton(
			UID												_creatorUid
		);

		UID createToolButtonCustomContextMenu(
			UID												_creatorUid,
			aToolButtonWidget *								_toolButton
		);

		//! @brief Will create a toolButton and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _text The initial text of the toolButton
		UID createToolButton(
			UID												_creatorUid,
			const QString &										_text
		);

		//! @brief Will create a toolButton and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _text The initial text of the toolButton
		//! @param _icon The initial icon of the toolButton
		UID createToolButton(
			UID												_creatorUid,
			const QString &										_text,
			const QIcon &										_icon
		);

		//! @brief Will create a tree and return its UID
		//! @param _creatorUid The UID of the creator
		UID createTree(
			UID												_creatorUid
		);

		//! @brief Will create a new tab view and return its UID
		//! @param _creatorUid The UID of the creator who creates this object
		UID createTabView(
			UID												_creatorUid
		);

		//! @brief Will create a tab toolbar sub container and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _parentUid The UID of the parent container or 
		//! @param _text The text of the new container
		UID createTabToolBarSubContainer(
			UID												_creatorUid,
			UID												_parentUid,
			const QString &										_text = QString("")
		);

		//! @brief Will create a tab toolbar page and return its UID
		//! @param _creatorUid The UID of the creator
		//! @param _uiManagerUid The UID of the ui manager where to create the page at
		//! @param _text The text of the page
		UID createTabToolBarPage(
			UID												_creatorUid,
			UID												_uiManagerUid,
			const QString &										_text = QString("")
		);

		/*
		//! @brief Will create a defaultWelcomeScreen and return its UID
		//! @param _creatorUid The UID of the creator
		UID createWelcomeScreen(
			UID												_creatorUid
		);
		*/

		//! @brief Will create a window and return its UID
		//! @param _creatorUid The UID of the creator
		UID createWindow(
			UID												_creatorUid
		);

		// ###############################################################################################################################################

		// Object manipulation

		//! @brief Will return the object with the specified object UID
		//! @param _objectUid The UID of the requested object
		aObject * object(
			UID												_objectUid
		);

		//! @brief Will return the object with the specified object unique name
		//! @param _objectUniqueName The unique name of the requested object
		aObject * object(
			const QString &										_objectUniqueName
		);

		// ###############################################################################################################################################

		// Special events

		//! @brief Will destroy all objects created by this creator
		//! @param _creatorUid The UID of the creator
		void creatorDestroyed(
			UID												_creatorUid
		);

		void destroy(
			UID												_objectUID,
			bool												_ignoreIfObjectHasChildObjects = false
		);

		void setIconSearchDirectories(
			const QStringList&							_paths
		);

		void addColorStyle(
			aColorStyle *								_colorStyle,
			bool												_activate
		);

		void setColorStyle(
			const QString &										_colorStyleName
		);

		void setDefaultColorStyle(void);

		void setDefaultDarkColorStyle(void);

		void setDefaultBlueColorStyle(void);

		aColorStyle * getCurrentColorStyle(void) const;

		QString getCurrentColorStyleName(void) const;

		//! @brief Will destroy all objects
		void destroyAll(void);

		/*
		//! @brief Will return the JSON settings string of all objects that have an alias set
		std::string saveStateWindow(
			const std::string &									_applicationVersion
		);
		*/

		//! @brief Will return the JSON settings string of all objects that have an alias set
		std::string saveStateColorStyle(
			const std::string &									_applicationVersion
		);

		/*
		//! @brief Will setup the UI with the settings provided in the settings JSON string
		//! @param _json The JSON string containing the settings
		//! @throw ak::Exception on syntax mismatch
		settingsRestoreErrorCode restoreStateWindow(
			const char *										_json,
			const std::string &									_applicationVersion
		);
		*/

		//! @brief Will setup the UI with the settings provided in the settings JSON string
		//! @param _json The JSON string containing the settings
		bool restoreStateColorStyle(
			const char *										_json,
			const std::string &									_applicationVersion
		);

		/*
		void addAlias(
			const QString &										_alias,
			UID												_UID
		);
		*/

		//! @brief Will set the unique name for the specified object.
		//! With the set name the object can be accessed later instead of the unique name.
		//! Should the unique name be empty, the object will not be acessable trough the name anymore.
		//! @param _objectUid The UID of the object
		//! @param _uniqueName The unique name to set
		void setObjectUniqueName(
			UID												_objectUid,
			const QString &										_uniqueName
		);

		/*

		void removeAlias(
			const QString &										_alias
		);

		*/

		//! @brief Will return the creator UID of the specified object
		//! @param _objectUID The UID of the object to get its cretor from
		UID objectCreator(
			UID												_objectUID
		);

		//! @brief Will return true if the specified object exists
		//! @param _objectUID The UID of the object
		bool objectExists(
			UID												_objectUID
		);

		//! @brief Will add the provided object to the paintable list
		//! @param _object The object to add
		void addPaintable(aPaintable * _object);

		//! @brief Will remove the object from the paintable list
		//! @param _object The object to remove
		void removePaintable(aPaintable * _object);

	private:

		void setColorStyle(
			aColorStyle *								_colorStyle
		);

		//! @brief Will register the created UID for the creator
		//! @param _creatorUid The UID of the creator
		//! @param _createdUid THe UID of the created object
		void addCreatedUid(
			UID												_creatorUid,
			UID												_createdUid
		);

		//! @brief Will cast and return the object to a QWidget
		QWidget * castToWidget(
			UID												_objectUid
		);		

		// ###############################################################################################################################################
		
		// Object storing

		std::map<UID, aObject *>			m_mapObjects;
		std::map<QString, aObject *>		m_mapUniqueNames;			//! Contains the UIDs for a specified unique name

		std::map<UID, std::vector<UID> *>	m_mapCreators;				//! Contains all creators
		std::map<UID, UID>					m_mapOwners;				//! Contains the UIDs of the owener of the objects

		std::vector<aColorStyle *>			m_colorStyles;
		
		std::map<aPaintable *, bool>		m_externalPaintableObjects;

		QStringList				m_iconSearchPaths;
		
		// ###############################################################################################################################################

		aUidManager *						m_uidManager;									//! The globally used uidManager
		aMessenger *						m_messenger;									//! The globally used messaging system
		aSignalLinker *						m_signalLinker;								//! The internal signal linker
		aColorStyle *						m_currentColorStyle;							//! The globally used color style
		aNotifierObjectManager *			m_notifier;									//! The notifier used to catch the destroyed messages

		aObjectManager() = delete;
		aObjectManager(const aObjectManager &) = delete;
		aObjectManager & operator = (aObjectManager &) = delete;

	};

	class UICORE_API_EXPORT ObjectNotFoundException : public std::exception {
	public:
		ObjectNotFoundException();
	};

} // namespace ak
