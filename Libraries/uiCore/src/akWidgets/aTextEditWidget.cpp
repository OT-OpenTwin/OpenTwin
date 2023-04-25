/*
 *	File:		aTextEditWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: March 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akGui/aColorStyle.h>
#include <akGui/aContextMenuItem.h>
#include <akWidgets/aTextEditWidget.h>

// Qt header
#include <qscrollbar.h>
#include <qmenu.h>
#include <qevent.h>

ak::aTextEditWidget::aTextEditWidget(QWidget * _parent)
: QTextEdit(_parent), aWidget(otTextEdit), m_autoScrollToBottom(false), m_contextMenu(nullptr), m_maxLength(0), m_controlIsPressed(false)
{ ini(); }
ak::aTextEditWidget::aTextEditWidget(const QString & _text, QWidget * _parent)
: QTextEdit(_text, _parent), aWidget(otTextEdit), m_autoScrollToBottom(false), m_contextMenu(nullptr), m_maxLength(0), m_controlIsPressed(false)
{ ini(); }

ak::aTextEditWidget::~aTextEditWidget()
{
	A_OBJECT_DESTROYING
	disconnect(this, SIGNAL(textChanged()), this, SLOT(slotChanged()));
}

// #######################################################################################################
// Event handling

void ak::aTextEditWidget::keyPressEvent(QKeyEvent *_event)
{
	if (_event->key() == Qt::Key_Control) { m_controlIsPressed = true; }
	if (m_maxLength <= 0) { QTextEdit::keyPressEvent(_event); emit keyPressed(_event); }
	else if (toPlainText().length() < m_maxLength || _event->key() == Qt::Key_Backspace || m_controlIsPressed ||
		_event->key() == Qt::Key_Up || _event->key() == Qt::Key_Down || _event->key() == Qt::Key_Left ||
		_event->key() == Qt::Key_Right) { QTextEdit::keyPressEvent(_event); emit keyPressed(_event); }
}

void ak::aTextEditWidget::keyReleaseEvent(QKeyEvent *_event) {
	if (_event->key() == Qt::Key_Control) { m_controlIsPressed = false; }
	if (m_maxLength <= 0) { QTextEdit::keyReleaseEvent(_event); emit keyReleased(_event); }
	else if (toPlainText().length() < m_maxLength || _event->key() == Qt::Key_Backspace || m_controlIsPressed ||
		_event->key() == Qt::Key_Up || _event->key() == Qt::Key_Down || _event->key() == Qt::Key_Left ||
		_event->key() == Qt::Key_Right) { QTextEdit::keyReleaseEvent(_event); emit keyReleased(_event); }
}

void ak::aTextEditWidget::focusInEvent(QFocusEvent * _event) {
	QTextEdit::focusInEvent(_event);
	emit focused();
}

void ak::aTextEditWidget::focusOutEvent(QFocusEvent * _event) {
	QTextEdit::focusOutEvent(_event);
	emit focusLost();
}

void ak::aTextEditWidget::slotChanged() {
	performAutoScrollToBottom();
}

void ak::aTextEditWidget::slotCustomMenuRequested(const QPoint & _pos) {
	if (m_contextMenuItems.size() != 0) { m_contextMenu->exec(viewport()->mapToGlobal(_pos)); }
}

void ak::aTextEditWidget::slotContextMenuItemClicked() {
	aContextMenuItem * item = nullptr;
	item = dynamic_cast<aContextMenuItem *>(sender());
	assert(item != nullptr); // Cast failed
	switch (item->role())
	{
	case cmrClear: clear(); break;
	case cmrNone: break;
	default:
		assert(0); // Unknown role
		break;
	}
	emit(contextMenuItemClicked(item->id()));
}

// #######################################################################################################

QWidget * ak::aTextEditWidget::widget(void) { return this; }

void ak::aTextEditWidget::setColorStyle(
	aColorStyle *			_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;
	
	QString sheet(m_colorStyle->toStyleSheet(cafForegroundColorControls |
		cafBackgroundColorControls | cafDefaultBorderWindow, "QTextEdit{", "}"));
	this->setStyleSheet(sheet);
	sheet = m_colorStyle->toStyleSheet(cafForegroundColorDialogWindow | cafBackgroundColorDialogWindow, "QMenu{", "}");
	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorDialogWindow | cafBackgroundColorDialogWindow, "QMenu::item{", "}"));
	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorFocus | cafBackgroundColorFocus, "QMenu::item:selected{", "}"));
	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorSelected | cafBackgroundColorSelected, "QMenu::item:pressed{", "}"));
	m_contextMenu->setStyleSheet(sheet);
}

// #######################################################################################################

// Context menu

ak::ID ak::aTextEditWidget::addContextMenuItem(
	aContextMenuItem *			_item
) {
	assert(_item != nullptr); // Nullptr provided
	_item->setId(++m_currentContextMenuItemId);
	m_contextMenuItems.push_back(_item);
	connect(_item, SIGNAL(triggered(bool)), this, SLOT(slotContextMenuItemClicked()));
	m_contextMenu->addAction(_item);
	if (m_colorStyle != nullptr) {
		QString sheet{ m_colorStyle->toStyleSheet(cafForegroundColorDialogWindow | cafBackgroundColorDialogWindow, "QMenu{", "}") };
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorDialogWindow | cafBackgroundColorDialogWindow, "QMenu::item{", "}"));
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorFocus | cafBackgroundColorFocus, "QMenu::item:selected{", "}"));
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorSelected | cafBackgroundColorSelected, "QMenu::item:pressed{", "}"));
		m_contextMenu->setStyleSheet(sheet);
	}
	return _item->id();
}

void ak::aTextEditWidget::addContextMenuSeparator(void) {
	m_contextMenu->addSeparator();
}

void ak::aTextEditWidget::clearContextMenu(void) {
	for (auto itm : m_contextMenuItems) {
		aContextMenuItem * item = itm;
		delete item;
	}
	m_contextMenuItems.clear();
	m_contextMenu->clear();
	m_currentContextMenuItemId = ak::invalidID;
}

// #######################################################################################################

void ak::aTextEditWidget::setAutoScrollToBottom(
	bool							_autoScroll
) { m_autoScrollToBottom = _autoScroll; }

bool ak::aTextEditWidget::autoScrollToBottom(void) const { return m_autoScrollToBottom; }

void ak::aTextEditWidget::performAutoScrollToBottom(void) {
	if (m_autoScrollToBottom) {
		//ensureCursorVisible();
		QScrollBar * bar = verticalScrollBar();
		if (bar->isVisible()) { bar->setValue(bar->maximum());}
	}
}

// #######################################################################################################

void ak::aTextEditWidget::ini(void) {
	m_currentContextMenuItemId = ak::invalidID;
	connect(this, SIGNAL(textChanged()), this, SLOT(slotChanged()));
	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(slotCustomMenuRequested(const QPoint &)));
	m_contextMenu = new QMenu(this);
	aContextMenuItem * newItem = new aContextMenuItem("Clear", cmrClear);
	addContextMenuItem(newItem);
}
