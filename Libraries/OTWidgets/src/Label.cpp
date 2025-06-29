//! @file Label.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Label.h"

// Qt header
#include <QtGui/qevent.h>

ot::Label::Label(QWidget* _parentWidget) : 
	QLabel(_parentWidget), m_mouseIsPressed(false)
{}

ot::Label::Label(const QString& _text, QWidget* _parentWidget) : 
	QLabel(_text, _parentWidget), m_mouseIsPressed(false)
{}

void ot::Label::mousePressEvent(QMouseEvent* _event) {
	if ((_event->button() == Qt::LeftButton) && this->isEnabled() && this->rect().contains(_event->pos())) {
		m_mouseIsPressed = true;
	}
}

void ot::Label::mouseReleaseEvent(QMouseEvent* _event) {
	if ((_event->button() == Qt::LeftButton)) {
		if (m_mouseIsPressed && this->isEnabled() && this->rect().contains(_event->pos())) {
			Q_EMIT mouseClicked();
		}

		m_mouseIsPressed = false;
	}
}

void ot::Label::mouseDoubleClickEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::LeftButton) {
		Q_EMIT mouseDoubleClicked();
	}
}
