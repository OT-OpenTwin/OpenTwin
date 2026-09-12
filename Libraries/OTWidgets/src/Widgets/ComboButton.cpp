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
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Widgets/ComboButton.h"

// Qt header
#include <QtWidgets/qmenu.h>

ot::ComboButton::ComboButton(QWidget* _parent)
	: PushButton(_parent)
{
	m_menu = new QMenu(this);

	ini();
}

ot::ComboButton::ComboButton(const QString& _text, QWidget* _parent)
	: PushButton(_text, _parent), m_text(_text)
{
	m_menu = new QMenu(this);
	m_menu->addAction(_text);
	
	ini();
}

ot::ComboButton::ComboButton(const QString& _text, const QStringList& _items, QWidget* _parent)
	: PushButton(_text, _parent), m_text(_text)
{
	m_menu = new QMenu(this);
	for (const QString& itm : _items)
	{
		m_menu->addAction(itm);
	}

	ini();
}

ot::ComboButton::~ComboButton()
{
	delete m_menu;
}

void ot::ComboButton::clear()
{
	m_menu->clear();
}

void ot::ComboButton::addItem(const QString& _item)
{
	m_menu->addAction(_item);
}

void ot::ComboButton::addItem(const QString& _item, const QVariant& _userData)
{
	QAction* action = m_menu->addAction(_item);
	action->setData(_userData);
}

void ot::ComboButton::setItems(const QStringList& _items)
{
	m_menu->clear();
	for (const QString& itm : _items)
	{
		m_menu->addAction(itm);
	}
}

void ot::ComboButton::setCurrentIndex(int _index)
{
	if (_index < 0 || _index >= m_menu->actions().size())
	{
		OT_LOG_ES("Invalid index for ComboButton: " << _index);
		return;
	}
	QAction* action = m_menu->actions().at(_index);
	if (action)
	{
		m_currentUserData = action->data();
		this->setText(action->text());
		Q_EMIT selectedItemChanged();
	}
	else
	{
		OT_LOG_ES("No action found at index " << _index << " for ComboButton");
	}
}

int ot::ComboButton::getFirstMatchingIndex(const QString& _text, Qt::CaseSensitivity _caseSensitivity) const
{
	QList<QAction*> actions = m_menu->actions();

	for (int i = 0; i < actions.size(); i++)
	{
		QAction* action = actions.at(i);
		OTAssertNullptr(action);
		if (action->text().compare(_text, _caseSensitivity) == 0)
		{
			return i;
		}
	}

	return -1;
}

void ot::ComboButton::setText(const QString& _text)
{
	if (m_text != _text)
	{
		m_text = _text;
		updateDisplayText();
	}
}

void ot::ComboButton::setPlaceholderText(const QString& _text)
{
	if (m_placeholderText != _text)
	{
		m_placeholderText = _text;
		updateDisplayText();
	}
}

void ot::ComboButton::setPlaceholderFont(const QFont& _font)
{
	m_placeholderFont = _font;
	updateDisplayText();
}

void ot::ComboButton::setFont(const QFont& _font)
{
	m_defaultFont = _font;
	updateDisplayText();
}

void ot::ComboButton::mousePressEvent(QMouseEvent* _event)
{
	PushButton::mousePressEvent(_event);
}

void ot::ComboButton::slotActionTriggered(QAction* _action)
{
	OTAssertNullptr(_action);
	if (_action->text() != getText())
	{
		m_currentUserData = _action->data();
		this->setText(_action->text());
		Q_EMIT selectedItemChanged();
	}
}

void ot::ComboButton::updateDisplayText()
{
	if (m_text.isEmpty())
	{
		m_state.set(State::PlaceholderShown);
		PushButton::setText(m_placeholderText);
		PushButton::setFont(m_placeholderFont);
		
	}
	else
	{
		m_state.remove(State::PlaceholderShown);
		PushButton::setText(m_text);
		PushButton::setFont(m_defaultFont);
		
	}
}

void ot::ComboButton::ini()
{
	m_state = State::None;

	setObjectName("OT_ComboButton");

	setMenu(m_menu);
	setFocusPolicy(Qt::NoFocus);

	connect(m_menu, &QMenu::triggered, this, &ComboButton::slotActionTriggered);
	connect(m_menu, &QMenu::aboutToShow, this, &ComboButton::setSelectedProperty);
	connect(m_menu, &QMenu::aboutToHide, this, &ComboButton::unsetSelectedProperty);

	m_defaultFont = this->font();

	m_placeholderFont = m_defaultFont;
	m_placeholderFont.setItalic(true);

	updateDisplayText();
}
