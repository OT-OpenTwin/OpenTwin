//! @file ToolTipHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/ToolTipHandler.h"

// Qt header
#include <QtWidgets/qtooltip.h>

void ot::ToolTipHandler::showToolTip(const QPoint& _pos, const QString& _text, int _timeout) {
	ToolTipHandler::instance().showToolTipImpl(_pos, _text, _timeout);
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