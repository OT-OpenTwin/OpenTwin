// @otlicense

//! @file EventNotifier.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTWidgets/EventNotifier.h"

// Qt header
#include <QtGui/qevent.h>

ot::EventNotifierMousePressed::EventNotifierMousePressed(QObject* _parent) 
	: QObject(_parent)
{}

bool ot::EventNotifierMousePressed::eventFilter(QObject* _obj, QEvent* _event) {
	if (_event->type() == QEvent::MouseButtonPress) {
		QMouseEvent* e = dynamic_cast<QMouseEvent*>(_event);
		OTAssertNullptr(e);
		if (e->button() == Qt::LeftButton) {
			Q_EMIT leftMouseButtonPressed();
		}
		else if (e->button() == Qt::MiddleButton) {
			Q_EMIT middleMouseButtonPressed();
		}
		else if (e->button() == Qt::RightButton) {
			Q_EMIT rightMouseButtonPressed();
		}
	}
	return false;
}
