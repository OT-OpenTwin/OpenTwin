// @otlicense
// File: InteractiveLabel.cpp
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
#include "OTWidgets/Widgets/InteractiveLabel.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qevent.h>

ot::InteractiveLabel::InteractiveLabel(QWidget* _parentWidget) 
	: Label(_parentWidget), m_mouseIsPressed(false)
{}

ot::InteractiveLabel::InteractiveLabel(const QString& _text, QWidget* _parentWidget)
	: Label(_text, _parentWidget), m_mouseIsPressed(false)
{}

void ot::InteractiveLabel::mousePressEvent(QMouseEvent* _event)
{
	if ((_event->button() == Qt::LeftButton) && this->isEnabled() && this->rect().contains(_event->pos()))
	{
		m_mouseIsPressed = true;
	}
}

void ot::InteractiveLabel::mouseReleaseEvent(QMouseEvent* _event)
{
	if ((_event->button() == Qt::LeftButton))
	{
		if (m_mouseIsPressed && this->isEnabled() && this->rect().contains(_event->pos()))
		{
			Q_EMIT mouseClicked(_event->modifiers());
		}

		m_mouseIsPressed = false;
	}
}

void ot::InteractiveLabel::mouseDoubleClickEvent(QMouseEvent* _event)
{
	if (_event->button() == Qt::LeftButton)
	{
		Q_EMIT mouseDoubleClicked(_event->modifiers());
	}
}

void ot::InteractiveLabel::enterEvent(QEnterEvent* _event)
{
	OT_UNUSED(_event);
	Q_EMIT mouseEntered();
}

void ot::InteractiveLabel::leaveEvent(QEvent* _event)
{
	OT_UNUSED(_event);
	Q_EMIT mouseLeft();
}
