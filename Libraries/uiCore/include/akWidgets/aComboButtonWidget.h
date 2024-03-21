/*
 *	File:		aComboButtonWidget.h
 *	Package:	akWidgets
 *
 *  Created on: March 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // Qt header
#include <qpushbutton.h>				// base class
#include <qstring.h>					// QString
#include <qstringlist.h>

// AK header
#include <akWidgets/aWidget.h>
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akWidgets/aComboButtonWidgetItem.h>
#include <akCore/aUidMangager.h>

// Forward declaration
class QMenu;
class QKeyEvent;

namespace ak {

	//! @brief This class represents a combo button which consists of a QPushButton and it's Menu
	//! This class behaves like a combo box
	class UICORE_API_EXPORT aComboButtonWidget : public QPushButton, public aWidget
	{
		Q_OBJECT
	public:
		//! @brief Constructor
		//! @param _initialText The initial text of this combo button
		//! @param _parent The parent QWidget for this combo button
		//! @throw ak::Exception if the creation of the menu failed
		aComboButtonWidget(
			const QString &									_initialText = QString(""),
			QWidget *										_parent = (QWidget *) nullptr
		);

		//! @brief Deconstructor
		virtual ~aComboButtonWidget();

		// #######################################################################################################
		// Event handling

		//! @brief Emits a key pressend signal a key is pressed
		virtual void keyPressEvent(QKeyEvent * _event) override;

		//! @brief Emits a key released signal a key is released
		virtual void keyReleaseEvent(QKeyEvent * _event) override;

		// #######################################################################################################

		//! @brief Will remove all items from the combo button
		void clearItems(void);

		//! @brief Will set the items for the combo button
		//! @param _menu The items to set as the menu for this combo button
		void setItems(
			const std::vector<QString> &					_menu
		);

		//! @brief Will set the items for the combo button
		//! @param _menu The items to set as the menu for this combo button
		void setItems(
			const QStringList &								_menu
		);

		//! @brief Will add a new item to the combo button
		//! @param _other The new item to add
		//! @throw ak::Exception if the creation of the new item failed
		ID addItem(
			const QString &									_text
		);

		//! @brief Will return the ammount of items in the combo button
		int getItemCount(void) const;

		//! @brief Will return the items
		const std::vector<aComboButtonWidgetItem *>& items(void) const { return m_items; }

		// #######################################################################################################
		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		QMenu* getMenu(void) { return m_menu; };

	signals:
		void keyPressed(QKeyEvent *);
		void keyReleased(QKeyEvent *);
		void changed(void);

	private slots:
		void slotItemTriggered();

	private:
		QMenu *													m_menu;				//! The QMenu of this combo button
		aUidManager												m_itemsUIDmanager;		//! The UID manager used for the menu items of this combo button
		std::vector<aComboButtonWidgetItem *>					m_items;				//! The items of this combo button
		std::map<UID, size_t>								m_itemUids;			//! The map containing the index of a item UID

		aComboButtonWidget(aComboButtonWidget &) = delete;
		aComboButtonWidget & operator = (aComboButtonWidget &) = delete;

	};
}
