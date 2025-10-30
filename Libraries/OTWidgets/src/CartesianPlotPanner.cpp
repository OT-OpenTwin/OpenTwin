// @otlicense

// OpenTwin header
#include "OTWidgets/CartesianPlot.h"
#include "OTWidgets/CartesianPlotPanner.h"

// Qt header
#include <QtGui/qevent.h>

ot::CartesianPlotPanner::CartesianPlotPanner(CartesianPlot* _plot)
	: QwtPlotPanner(_plot->canvas()), m_mouseIsPressed(false) 
{
	this->setMouseButton(Qt::MouseButton::MiddleButton);
}

ot::CartesianPlotPanner::~CartesianPlotPanner() {}

bool ot::CartesianPlotPanner::eventFilter(QObject* _object, QEvent* _event) {
	if (_object == nullptr || _object != parentWidget()) {
		return false;
	}

	switch (_event->type()) {
	case QEvent::MouseButtonPress:
	{
		auto evr = static_cast<QMouseEvent*>(_event);
		if (evr->button() == Qt::MouseButton::MiddleButton) {
			m_mouseIsPressed = true;
			this->widgetMousePressEvent(evr);
		}
		break;
	}
	case QEvent::MouseMove:
	{
		QMouseEvent* evr = static_cast<QMouseEvent*>(_event);
		if (m_mouseIsPressed) {
			this->widgetMouseMoveEvent(evr);
			this->widgetMouseReleaseEvent(evr);
			this->setMouseButton(evr->button(), evr->modifiers());
			this->widgetMousePressEvent(evr);
		}
		break;
	}
	case QEvent::MouseButtonRelease:
	{
		QMouseEvent* evr = static_cast<QMouseEvent*>(_event);
		if (evr->button() == Qt::MouseButton::MiddleButton) {
			m_mouseIsPressed = false;
			this->widgetMouseReleaseEvent(evr);
		}
		break;
	}
	case QEvent::KeyPress:
	{
		//this->widgetKeyPressEvent(static_cast<QKeyEvent *>(_event));
		break;
	}
	case QEvent::KeyRelease:
	{
		//this->widgetKeyReleaseEvent(static_cast<QKeyEvent *>(_event));
		break;
	}
	case QEvent::Paint:
	{
		if (isVisible())
			return true;
		break;
	}
	default:;
	}

	return false;
}
