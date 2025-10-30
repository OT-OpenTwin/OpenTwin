// @otlicense
// File: ComboButton.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/ComboButton.h"

// Qt header
#include <QtWidgets/qmenu.h>

ot::ComboButton::ComboButton(QWidget* _parent)
	: PushButton(_parent)
{
	m_menu = new QMenu(this);

	this->ini();
}

ot::ComboButton::ComboButton(const QString& _text, QWidget* _parent) 
	: PushButton(_text, _parent)
{
	m_menu = new QMenu(this);
	m_menu->addAction(_text);

	this->ini();
}

ot::ComboButton::ComboButton(const QString& _text, const QStringList& _items, QWidget* _parent)
	: PushButton(_text, _parent)
{
	m_menu = new QMenu(this);
	for (const QString& itm : _items) {
		m_menu->addAction(itm);
	}

	this->ini();
}

ot::ComboButton::~ComboButton() {
	delete m_menu;
}

void ot::ComboButton::setItems(const QStringList& _items) {
	m_menu->clear();
	for (const QString& itm : _items) {
		m_menu->addAction(itm);
	}
}

void ot::ComboButton::mousePressEvent(QMouseEvent* _event) {
	PushButton::mousePressEvent(_event);
}

void ot::ComboButton::slotActionTriggered(QAction* _action) {
	this->setText(_action->text());
	Q_EMIT textChanged();
}

void ot::ComboButton::ini(void) {
	this->setObjectName("OT_ComboButton");

	this->setMenu(m_menu);
	this->setFocusPolicy(Qt::NoFocus);

	this->connect(m_menu, &QMenu::triggered, this, &ComboButton::slotActionTriggered);
	this->connect(m_menu, &QMenu::aboutToShow, this, &ComboButton::setSelectedProperty);
	this->connect(m_menu, &QMenu::aboutToHide, this, &ComboButton::unsetSelectedProperty);
}
