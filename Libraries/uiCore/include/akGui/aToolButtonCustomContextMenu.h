/*
 *	File:		aToolButtonCustomContextMenu.h
 *	Package:	akGui
 *
 *  Created on: October 29, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // AK header
#include <akCore/globalDataTypes.h>
#include <akCore/aObject.h>

// Qt header
#include <qstring.h>					// QString
#include <qicon.h>						// QIcon
#include <qpoint.h>
#include <qobject.h>

class QMenu;

namespace ak {

	class aContextMenuItem;
	class aToolButtonWidget;

	class UICORE_API_EXPORT aToolButtonCustomContextMenu : public QObject, public aObject {
		Q_OBJECT
	public:

		aToolButtonCustomContextMenu(aToolButtonWidget * _button);

		virtual ~aToolButtonCustomContextMenu();

		// #######################################################################################################

		//! @brief Will add a new menu item to the menu
		//! This toolButton will take over control over the contextMenuItem
		//! @param _item The item to add
		ID addMenuItem(
			aContextMenuItem *					_item
		);

		//! @brief Will add a menu seperator to the menu
		void addMenuSeperator(void);

		//! @brief Will clear the menu
		void clearMenu(void);

		//! @brief Will set the checked state of the specified menu item
		//! @param _itemID The ID of the item
		//! @param _checked The checked state to set
		void setMenuItemChecked(
			ID								_itemID,
			bool								_checked = true
		);

		//! @brief Will disable the ability to check and uncheck the item (can be reenabled with setChecked)
		//! @param _itemID The ID of the item
		void setMenuItemNotCheckable(
			ID								_itemID
		);

		//! @brief Will return the text of the specified menu item
		//! @param _itemID The ID of the menu item
		QString getMenuItemText(
			ID								_itemID
		);

		aContextMenuItem * item(ID _itemId);

	signals:
		void menuItemClicked(ID);
		void menuItemCheckedChanged(ID, bool);

	private slots:
		void slotMenuRequested(const QPoint& _pos);
		void slotMenuItemClicked(void);
		void slotMenuItemCheckedChanged(bool _state);

	private:

		ID								m_itemId;
		QMenu *							m_menu;
		std::vector<aContextMenuItem *>	m_menuItems;
		aToolButtonWidget *				m_button;

	};
}