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

// AK header
#include <akGui/aAction.h>
#include <akGui/aColorStyle.h>
#include <akGui/aContextMenuItem.h>
#include <akWidgets/aToolButtonWidget.h>

// Qt header
#include <qmenu.h>
#include <qevent.h>

ak::aToolButtonWidget::aToolButtonWidget()
	: QToolButton(), aWidget(otToolButton),
	m_action(nullptr)
{
	m_action = new aAction();
	ini();
}

ak::aToolButtonWidget::aToolButtonWidget(
	const QString &				_text
)	: QToolButton(), ak::aWidget(otToolButton),
	m_action(nullptr)
{
	m_action = new aAction(_text);
	ini();
}

ak::aToolButtonWidget::aToolButtonWidget(
	const QIcon &				_icon,
	const QString &				_text
)	: QToolButton(), ak::aWidget(otToolButton),
	m_action(nullptr)
{
	m_action = new aAction(_icon, _text);
	ini();
}

ak::aToolButtonWidget::~aToolButtonWidget() { A_OBJECT_DESTROYING }

// #######################################################################################################

// Event handling

void ak::aToolButtonWidget::keyPressEvent(QKeyEvent *_event)
{
	QToolButton::keyPressEvent(_event);
	emit keyPressed(_event);
}

void ak::aToolButtonWidget::keyReleaseEvent(QKeyEvent * _event) {
	QToolButton::keyReleaseEvent(_event);
	emit keyReleased(_event);
}

// #######################################################################################################

// Base class function

void ak::aToolButtonWidget::setColorStyle(
	aColorStyle *			_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;

	QString sheet(m_colorStyle->toStyleSheet(cafForegroundColorControls |
		cafBackgroundColorControls, "QToolButton{", "}"));

	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorFocus |
		cafBackgroundColorFocus, "QToolButton:hover:!pressed{", "}"));

	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorSelected |
		cafBackgroundColorSelected, "QToolButton:pressed{", "}"));

	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
		cafBackgroundColorHeader | cafBorderColorHeader | cafDefaultBorderWindow,
		"QToolTip{", "}"));
	setStyleSheet(sheet);

	if (m_menu != nullptr) {
		sheet = m_colorStyle->toStyleSheet(cafForegroundColorDialogWindow | cafBackgroundColorDialogWindow, "QMenu{", "}");
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorDialogWindow | cafBackgroundColorDialogWindow, "QMenu::item{", "}"));
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorFocus | cafBackgroundColorFocus, "QMenu::item:selected{", "}"));
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorSelected | cafBackgroundColorSelected, "QMenu::item:pressed{", "}"));
		m_menu->setStyleSheet(sheet);
	}
}

// #######################################################################################################

void ak::aToolButtonWidget::SetToolTip(
	const QString &						_text
) {
	setToolTip(_text);
	setWhatsThis(_text);
	m_action->setToolTip(_text);
	m_action->setWhatsThis(_text);
}

QString ak::aToolButtonWidget::ToolTip(void) const { return toolTip(); }

ak::ID ak::aToolButtonWidget::addMenuItem(
	aContextMenuItem *					_item
) {
	if (m_menu == nullptr) {
		m_menu = new QMenu();
		setMenu(m_menu);
	}
	_item->setId(m_menuItems.size());
	m_menuItems.push_back(_item);

	m_menu->addAction(_item);

	connect(_item, SIGNAL(triggered(bool)), this, SLOT(slotMenuItemClicked()));
	connect(_item, SIGNAL(toggled(bool)), this, SLOT(slotMenuItemCheckedChanged()));

	return _item->id();
}

void ak::aToolButtonWidget::addMenuSeperator(void) {
	if (m_menu == nullptr) {
		m_menu = new QMenu();
		setMenu(m_menu);
	}
	m_menu->addSeparator();
}

void ak::aToolButtonWidget::clearMenu(void) {
	if (m_menu != nullptr) {
		for (auto itm : m_menuItems) { aContextMenuItem * item = itm; delete itm; }
		setMenu(nullptr);
		delete m_menu;
		m_menu = nullptr;
		m_menuItems.clear();
	}
}

void ak::aToolButtonWidget::setMenuItemChecked(
	ak::ID								_itemID,
	bool								_checked
) {
	assert(_itemID >= 0 && _itemID < m_menuItems.size());	// Index out of range
	aContextMenuItem * itm = m_menuItems[_itemID];
	itm->setCheckable(true);
	itm->setChecked(_checked);
}

void ak::aToolButtonWidget::setMenuItemNotCheckable(
	ak::ID								_itemID
) {
	assert(_itemID >= 0 && _itemID < m_menuItems.size());	// Index out of range
	aContextMenuItem * itm = m_menuItems[_itemID];
	itm->setCheckable(false);
}

QString ak::aToolButtonWidget::getMenuItemText(
	ak::ID								_itemID
) {
	assert(_itemID >= 0 && _itemID < m_menuItems.size());	// Index out of range
	aContextMenuItem * itm = m_menuItems[_itemID];
	return itm->text();
}

// #######################################################################################################

// Slots

void ak::aToolButtonWidget::slotClicked() {
	if (m_menuItems.size() != 0) {
		showMenu();
	}
	else { emit btnClicked(); }
}

void ak::aToolButtonWidget::slotMenuItemClicked() {
	aContextMenuItem * itm = nullptr;
	itm = dynamic_cast<aContextMenuItem *>(sender());
	assert(itm != nullptr); // Cast failed
	emit menuItemClicked(itm->id());
}

void ak::aToolButtonWidget::slotMenuItemCheckedChanged() {
	aContextMenuItem * itm = nullptr;
	itm = dynamic_cast<aContextMenuItem *>(sender());
	assert(itm != nullptr); // Cast failed
	emit menuItemCheckedChanged(itm->id(), itm->isChecked());
}

// #######################################################################################################

// Private member

void ak::aToolButtonWidget::ini(void) {
	setDefaultAction(m_action);
	setToolTip("");
	setWhatsThis("");
	setFocusPolicy(Qt::FocusPolicy::NoFocus);
	m_action->setToolTip("");
	m_action->setWhatsThis("");
	m_menu = nullptr;
	m_action->setMenuRole(QAction::MenuRole::NoRole);
	m_action->setIconVisibleInMenu(true);
	connect(m_action, &aAction::triggered, this, &aToolButtonWidget::slotClicked);
}
