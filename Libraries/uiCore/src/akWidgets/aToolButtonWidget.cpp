// @otlicense
// File: aToolButtonWidget.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// AK header
#include <akGui/aAction.h>
#include <akWidgets/aToolButtonWidget.h>

// Qt header
#include <qmenu.h>
#include <qevent.h>

ak::aToolButtonWidget::aToolButtonWidget(QWidget* _parent)
	: QToolButton(_parent), aWidget(otToolButton),
	m_action(nullptr)
{
	m_action = new aAction(QToolButton::InstantPopup, this);
	ini();
}

ak::aToolButtonWidget::aToolButtonWidget(const QString& _text, QWidget* _parent) : QToolButton(_parent), ak::aWidget(otToolButton),
	m_action(nullptr)
{
	m_action = new aAction(_text, QToolButton::InstantPopup, this);
	ini();
}

ak::aToolButtonWidget::aToolButtonWidget(const QIcon& _icon, const QString& _text, QWidget* _parent) : QToolButton(_parent), ak::aWidget(otToolButton),
	m_action(nullptr)
{
	m_action = new aAction(_icon, _text, QToolButton::InstantPopup, this);
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
