//! @file PolarPlotMagnifier.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PolarPlot.h"
#include "OTWidgets/PolarPlotMarker.h"
#include "OTWidgets/PolarPlotMagnifier.h"

// Qt header
#include <QtGui/qevent.h>

ot::PolarPlotMagnifier::PolarPlotMagnifier(PolarPlot* _plot)
	: QwtPolarMagnifier(_plot->canvas()), m_rightMouseIsPressed(false), m_mouseMoved(false), m_plot(_plot) 
{
	this->setMouseButton(Qt::MouseButton::NoButton);
	this->setEnabled(true);
	m_marker = new PolarPlotMarker(m_plot);
	m_plot->setVisible(false);
}

ot::PolarPlotMagnifier::~PolarPlotMagnifier() {

}

void ot::PolarPlotMagnifier::widgetMousePressEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		m_rightMouseIsPressed = true;
		m_mouseMoved = false;
	}
	QwtPolarMagnifier::widgetMousePressEvent(_event);
}

void ot::PolarPlotMagnifier::widgetMouseMoveEvent(QMouseEvent* _event) {

	m_mouseMoved = true;

	QwtPolarMagnifier::widgetMouseMoveEvent(_event);
}

void ot::PolarPlotMagnifier::widgetMouseReleaseEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		if (m_mouseMoved) {
			m_marker->setVisible(false);
			m_plot->replot();
		}
		m_rightMouseIsPressed = false;
	}
	QwtPolarMagnifier::widgetMouseReleaseEvent(_event);
}

void ot::PolarPlotMagnifier::widgetWheelEvent(QWheelEvent* _wheelEvent) {
	if (_wheelEvent->modifiers() != wheelModifiers()) { return; }

	if (this->wheelFactor() != 0.0) {
		int delta = _wheelEvent->angleDelta().y() * (-1);
		double f = qPow(wheelFactor(), qAbs(delta / 120.0));
		if (delta > 0) {
			f = 1 / f;
		}

		this->rescale(f);
	}
}

void ot::PolarPlotMagnifier::rescale(double _factor) {
	QwtPolarMagnifier::rescale(_factor);
}
