// @otlicense
// File: ToolButton.cpp
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

// OpenTwin header
#include "OTWidgets/Action.h"
#include "OTWidgets/ToolButton.h"

ot::ToolButton::ToolButton(QWidget* _parent) : QToolButton(_parent), m_action(nullptr) {
	m_action = new Action;
	this->ini();
}

ot::ToolButton::ToolButton(const QString& _text, QWidget* _parent)
	: QToolButton(_parent), m_action(nullptr) 
{
	m_action = new Action(_text);
	this->ini();
}

ot::ToolButton::ToolButton(const QIcon& _icon, const QString& _text, QWidget* _parent)
	: QToolButton(_parent), m_action(nullptr) 
{
	m_action = new Action(_icon, _text);
	this->ini();
}

ot::ToolButton::~ToolButton() {}

// #######################################################################################################

// Private member

void ot::ToolButton::ini(void) {
	this->setDefaultAction(m_action);
	this->setToolTip(QString());
	this->setWhatsThis(QString());
	this->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	m_action->setToolTip(QString());
	m_action->setWhatsThis(QString());
	m_action->setMenuRole(QAction::MenuRole::NoRole);
	m_action->setIconVisibleInMenu(true);
}
