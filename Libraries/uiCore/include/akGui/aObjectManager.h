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
	class aUidManager;
	class aSignalLinker;
	class aGlobalKeyListener;
	class aToolButtonWidget;

	//! @brief The objectManager is used for creating and manipulating objects and widgets
	//! All objects created here a derived from ak::core::aObject or child
	class UICORE_API_EXPORT aObjectManager {
	public:
		//! @brief Constructor
		//! @param _messenger The globally used messaging system
		//! @param _uidManager The globally used uidManager
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

		//! @brief Will destroy all objects
		void destroyAll(void);

		//! @brief Will set the unique name for the specified object.
		//! With the set name the object can be accessed later instead of the unique name.
		//! Should the unique name be empty, the object will not be acessable trough the name anymore.
		//! @param _objectUid The UID of the object
		//! @param _uniqueName The unique name to set
		void setObjectUniqueName(
			UID												_objectUid,
			const QString &										_uniqueName
		);

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

	private:

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

		// ###############################################################################################################################################

		aUidManager *						m_uidManager;									//! The globally used uidManager
		aMessenger *						m_messenger;									//! The globally used messaging system
		aSignalLinker *						m_signalLinker;								//! The internal signal linker
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
