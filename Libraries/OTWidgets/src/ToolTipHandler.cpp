// @otlicense
// File: ToolTipHandler.cpp
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
#include "OTWidgets/ToolTipHandler.h"

// Qt header
#include <QtWidgets/qtooltip.h>
#include <QtGui/qfontmetrics.h>

void ot::ToolTipHandler::showToolTip(const QPoint& _pos, const QString& _text, int _timeout) {
	ToolTipHandler::instance().showToolTipImpl(_pos, _text, _timeout);
}

void ot::ToolTipHandler::showToolTip(const QWidget* _widget, const QString& _text, Alignment _alignment, int _timeout) {	
	OTAssertNullptr(_widget);

	QPoint globalPos;
	switch (_alignment) {
	case Alignment::TopLeft:
		globalPos = _widget->mapToGlobal(QPoint(0, 0));
		break;
	case Alignment::TopRight:
		globalPos = _widget->mapToGlobal(QPoint(_widget->width(), 0));
		break;
	case Alignment::BottomLeft:
		globalPos = _widget->mapToGlobal(QPoint(0, _widget->height()));
		break;
	case Alignment::BottomRight:
		globalPos = _widget->mapToGlobal(QPoint(_widget->width(), _widget->height()));
		break;
	case Alignment::Center:
		globalPos = _widget->mapToGlobal(QPoint(_widget->width() / 2, _widget->height() / 2));
		break;
	case Alignment::Top:
		globalPos = _widget->mapToGlobal(QPoint(_widget->width() / 2, 0));
		break;
	case Alignment::Bottom:
		globalPos = _widget->mapToGlobal(QPoint(_widget->width() / 2, _widget->height()));
		break;
	case Alignment::Left:
		globalPos = _widget->mapToGlobal(QPoint(0, _widget->height() / 2));
		break;
	case Alignment::Right:
		globalPos = _widget->mapToGlobal(QPoint(_widget->width(), _widget->height() / 2));
		break;
	default:
		globalPos = _widget->mapToGlobal(QPoint(0, 0));
		break;
	}

	ToolTipHandler::instance().showToolTipImpl(globalPos, _text, _timeout);
}

void ot::ToolTipHandler::hideToolTip(void) {
	ToolTipHandler::instance().hideToolTipImpl();
}

void ot::ToolTipHandler::slotShowDelayedToolTip(void) {
	QToolTip::showText(m_pos, m_text);
}

ot::ToolTipHandler& ot::ToolTipHandler::instance(void) {
	static ot::ToolTipHandler g_instance;
	return g_instance;
}

void ot::ToolTipHandler::showToolTipImpl(const QPoint& _pos, const QString& _text, int _timeout) {
	this->hideToolTipImpl();

	m_pos = _pos;
	m_text = _text;
	if (_timeout) {
		m_timer.setInterval(_timeout);
		m_timer.start();
	}
	else {
		this->slotShowDelayedToolTip();
	}
}

void ot::ToolTipHandler::hideToolTipImpl(void) {
	m_timer.stop();
	QToolTip::hideText();
}

ot::ToolTipHandler::ToolTipHandler() {
	m_timer.setSingleShot(true);
	this->connect(&m_timer, &QTimer::timeout, this, &ot::ToolTipHandler::slotShowDelayedToolTip);
}

ot::ToolTipHandler::~ToolTipHandler() {

}