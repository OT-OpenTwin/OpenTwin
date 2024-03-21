/*
 *	File:		aToolButtonWidget.h
 *	Package:	akWidgets
 *
 *  Created on: November 07, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // AK header
#include <akCore/globalDataTypes.h>
#include <akWidgets/aWidget.h>

// Qt header
#include <qtoolbutton.h>				// Base class
#include <qstring.h>					// QString
#include <qicon.h>						// QIcon
#include <qpoint.h>

class QMenu;

namespace ak {

	class aAction;
	class aContextMenuItem;

	class UICORE_API_EXPORT aToolButtonWidget : public QToolButton, public aWidget {
		Q_OBJECT
	public:
		aToolButtonWidget();

		aToolButtonWidget(
			const QString &				_text
		);

		aToolButtonWidget(
			const QIcon &				_icon,
			const QString &				_text
		);

		virtual ~aToolButtonWidget();

		// #######################################################################################################

		// Event handling

		//! @brief Emits a key pressend signal a key is pressed
		virtual void keyPressEvent(QKeyEvent * _event) override;

		//! @brief Emits a key released signal a key is released
		virtual void keyReleaseEvent(QKeyEvent * _event) override;

		// #######################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override { return this; }

		// #######################################################################################################

		//! @brief Will return the action of this tool button
		aAction * getAction() const { return m_action; }

		//! @brief Will set the toolTip and WhatsThis of this toolButton and its action
		//! @param _text The text to set
		void SetToolTip(
			const QString &						_text
		);

		//! @brief Will return the current toolTip of this toolButton
		QString ToolTip(void) const;

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

	signals:
		void btnClicked();
		void keyPressed(QKeyEvent *);
		void keyReleased(QKeyEvent *);
		void menuItemClicked(ID);
		void menuItemCheckedChanged(ID, bool);

	private slots:
		void slotClicked();
		void slotMenuItemClicked();
		void slotMenuItemCheckedChanged();

	private:

		//! @brief Initializes the components of this toolButton
		void ini(void);

		QMenu *							m_menu;
		std::vector<aContextMenuItem *>	m_menuItems;
		aAction *						m_action;

	};
}