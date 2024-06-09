//! @file Label.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Label.h"

// Qt header
#include <QtGui/qevent.h>

ot::Label::Label(QWidget* _parentWidget) : QLabel(_parentWidget) {}

ot::Label::Label(const QString& _text, QWidget* _parentWidget) : QLabel(_text, _parentWidget) {}

void ot::Label::mousePressEvent(QMouseEvent* _event) {
	if ((_event->button() == Qt::LeftButton) && this->isEnabled()) {
		Q_EMIT mousePressed();
	}
}
