/*
 *	File:		aToolButtonWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: November 07, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akGui/aToolButtonCustomContextMenu.h>
#include <akWidgets/aToolButtonWidget.h>
#include <akGui/aContextMenuItem.h>
#include <akCore/aAssert.h>
#include <qmenu.h>

ak::aToolButtonCustomContextMenu::aToolButtonCustomContextMenu(aToolButtonWidget * _button) 
	: aObject(otToolButtonCustomContextMenu), m_button(_button), m_itemId(invalidID)
{
	m_menu = new QMenu;
	m_button->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_button, &aToolButtonWidget::customContextMenuRequested, this, &aToolButtonCustomContextMenu::slotMenuRequested);
}

ak::aToolButtonCustomContextMenu::~aToolButtonCustomContextMenu() {

}

// #######################################################################################################

ak::ID ak::aToolButtonCustomContextMenu::addMenuItem(
	aContextMenuItem *					_item
) {
	_item->setId(++m_itemId);
	m_menuItems.push_back(_item);
	m_menu->addAction(_item);
	connect(_item, &aContextMenuItem::triggered, this, &aToolButtonCustomContextMenu::slotMenuItemClicked);
	connect(_item, &aContextMenuItem::toggled, this, &aToolButtonCustomContextMenu::slotMenuItemCheckedChanged);
	return m_itemId;
}

void ak::aToolButtonCustomContextMenu::addMenuSeperator(void) {
	m_menu->addSeparator();
}

void ak::aToolButtonCustomContextMenu::clearMenu(void) {
	bool erased{ true };
	while (erased) {
		erased = false;
		if (!m_menuItems.empty()) {
			delete m_menuItems[0];
			m_menuItems.erase(m_menuItems.begin());
			erased = true;
		}
	}
	m_menu->clear();
	m_menuItems.clear();
	m_itemId = invalidID;
}

void ak::aToolButtonCustomContextMenu::setMenuItemChecked(
	ID									_itemID,
	bool								_checked
) {
	m_menuItems[_itemID]->setChecked(_checked);
}

void ak::aToolButtonCustomContextMenu::setMenuItemNotCheckable(
	ID								_itemID
) {
	m_menuItems[_itemID]->setCheckable(false);
}

QString ak::aToolButtonCustomContextMenu::getMenuItemText(
	ID								_itemID
) {
	return m_menuItems[_itemID]->text();
}

ak::aContextMenuItem * ak::aToolButtonCustomContextMenu::item(ID _itemId) {
	for (auto itm : m_menuItems) {
		if (itm->id() == _itemId) { return itm; }
	}
	aAssert(0, "Invalid item id");
	return nullptr;
}

// #######################################################################################################

// Slots

void ak::aToolButtonCustomContextMenu::slotMenuRequested(const QPoint& _pos) {
	QPoint globalPos = m_button->mapToGlobal(_pos);
	//m_menu->move(globalPos);
	m_menu->exec(globalPos);
}

void ak::aToolButtonCustomContextMenu::slotMenuItemClicked() {
	aContextMenuItem * itm = nullptr;
	itm = dynamic_cast<aContextMenuItem *>(sender());
	assert(itm != nullptr); // Cast failed
	Q_EMIT menuItemClicked(itm->id());
}

void ak::aToolButtonCustomContextMenu::slotMenuItemCheckedChanged(bool _state) {
	aContextMenuItem * itm = nullptr;
	itm = dynamic_cast<aContextMenuItem *>(sender());
	assert(itm != nullptr); // Cast failed
	Q_EMIT menuItemCheckedChanged(itm->id(), itm->isChecked());
}
