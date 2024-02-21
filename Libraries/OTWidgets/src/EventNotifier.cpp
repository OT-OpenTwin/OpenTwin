//! @file EventNotifier.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTAssert.h"
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
			emit leftMouseButtonPressed();
		}
		else if (e->button() == Qt::MiddleButton) {
			emit middleMouseButtonPressed();
		}
		else if (e->button() == Qt::RightButton) {
			emit rightMouseButtonPressed();
		}
	}
	return false;
}