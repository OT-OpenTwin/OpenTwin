// @otlicense

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
	Q_EMIT keyPressed(_event);
}

void ak::aToolButtonWidget::keyReleaseEvent(QKeyEvent * _event) {
	QToolButton::keyReleaseEvent(_event);
	Q_EMIT keyReleased(_event);
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

// #######################################################################################################

// Slots

void ak::aToolButtonWidget::slotClicked() {
	Q_EMIT btnClicked();
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
	m_action->setMenuRole(QAction::MenuRole::NoRole);
	m_action->setIconVisibleInMenu(true);
	connect(m_action, &aAction::triggered, this, &aToolButtonWidget::slotClicked);
}
