/*
 *	File:		aDockWatcherWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: June 30, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akWidgets/aDockWatcherWidget.h>
#include <akGui/aColorStyle.h>
#include <akGui/aContextMenuItem.h>

#include <qdockwidget.h>
#include <qmenu.h>

ak::aDockWatcherWidget::aDockWatcherWidget(const QString & _title)
	: aToolButtonWidget(_title), m_isEnabled(true)
{
	m_menu = new QMenu;
	setMenu(m_menu);
	connect(this, &QToolButton::clicked, this, &aDockWatcherWidget::slotClicked);
	//m_objectType = otDockWatcher;
}

ak::aDockWatcherWidget::aDockWatcherWidget(const QIcon & _icon, const QString & _title)
	: aToolButtonWidget(_icon, _title), m_isEnabled(true)
{
	m_menu = new QMenu;
	setMenu(m_menu);
	connect(this, &QToolButton::clicked, this, &aDockWatcherWidget::slotClicked);
	//m_objectType = otDockWatcher;
}

ak::aDockWatcherWidget::~aDockWatcherWidget() {

}

// #######################################################################################################

QWidget * ak::aDockWatcherWidget::widget(void) { return this; }

void ak::aDockWatcherWidget::setColorStyle(
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
		cafBackgroundColorHeader | cafBorderColorHeader,
		"QToolTip{", "border: 1px;}"));
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

void ak::aDockWatcherWidget::refreshData(void) {
	for (auto itm : m_dockMap) {
		itm.second->setChecked(itm.first->isVisible());
	}
}

void ak::aDockWatcherWidget::addWatch(QDockWidget * _dock) {
	auto action = new aContextMenuItem(_dock->windowTitle(), cmrNone);
	m_menu->addAction(action);
	action->setCheckable(true);
	action->setChecked(_dock->isVisible());
	m_dockMap.insert_or_assign(_dock, action);
	m_actionMap.insert_or_assign(action, _dock);

	connect(_dock, &QDockWidget::visibilityChanged, this, &aDockWatcherWidget::slotVisibilityChanged);
	connect(action, &QAction::toggled, this, &aDockWatcherWidget::slotMenuItemChanged);
}

void ak::aDockWatcherWidget::removeWatch(QDockWidget * _dock) {
	auto itm = m_dockMap.find(_dock);
	assert(itm != m_dockMap.end());
	m_actionMap.erase(itm->second);
	disconnect(_dock, &QDockWidget::visibilityChanged, this, &aDockWatcherWidget::slotVisibilityChanged);
	disconnect(itm->second, &QAction::toggled, this, &aDockWatcherWidget::slotMenuItemChanged);
	m_menu->removeAction(itm->second);
	delete itm->second;
	m_dockMap.erase(_dock);
}

void ak::aDockWatcherWidget::slotVisibilityChanged(bool _visible) {
	if (!m_isEnabled) { return; }
	QDockWidget * actualDock = nullptr;
	actualDock = dynamic_cast<QDockWidget *>(sender());
	assert(actualDock != nullptr);	// Sender is not a dock
	auto itm = m_dockMap.find(actualDock);
	assert(itm != m_dockMap.end());
	itm->second->setChecked(actualDock->isVisible());
}

void ak::aDockWatcherWidget::slotClicked() {
	if (m_dockMap.size() > 0) {
		showMenu();
	}
}

void ak::aDockWatcherWidget::slotMenuItemChanged() {
	aContextMenuItem * itm = nullptr;
	itm = dynamic_cast<aContextMenuItem *>(sender());
	assert(itm != nullptr);
	auto d = m_actionMap.find(itm);
	assert(d != m_actionMap.end());
	d->second->setVisible(itm->isChecked());
}