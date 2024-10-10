//! @file CartesianPlotMagnifier.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Plot.h"
#include "OTWidgets/PlotDataset.h"
#include "OTWidgets/CartesianPlot.h"
#include "OTWidgets/CartesianPlotMarker.h"
#include "OTWidgets/CartesianPlotMagnifier.h"

// Qwt header
#include <qwt_text.h>
#include <qwt_scale_map.h>

// Qt header
#include <QtGui/qevent.h>

ot::CartesianPlotMagnifier::CartesianPlotMagnifier(CartesianPlot* _plot)
	: QwtPlotMagnifier(_plot->canvas()), m_rightMouseIsPressed(false), m_mouseMoved(false), m_plot(_plot) 
{
	m_marker = new CartesianPlotMarker(0);
	m_marker->attach(m_plot);
	m_marker->setVisible(false);
	m_marker->setLinePen(QColor(255, 50, 50), 0.0, Qt::DashDotDotLine);

	this->setMouseButton(Qt::MouseButton::NoButton);
}

void ot::CartesianPlotMagnifier::widgetMousePressEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		m_rightMouseIsPressed = true;
		m_mouseMoved = false;
	}
	QwtPlotMagnifier::widgetMousePressEvent(_event);
}

void ot::CartesianPlotMagnifier::widgetMouseMoveEvent(QMouseEvent* _event) {

	m_mouseMoved = true;

	if (m_rightMouseIsPressed) {
		int itemId;
		QwtPlotCurve* curve = m_plot->findNearestCurve(_event->pos(), itemId);

		if (curve != nullptr) {
			double x, y;
			m_plot->getOwner()->findDataset(curve)->getDataAt(itemId, x, y);
			m_marker->setValue(x, y);
			m_marker->setLabel(QwtText((std::to_string(x) + ", " + std::to_string(y)).c_str()));
			m_marker->setVisible(true);
			m_plot->replot();
		}
	}
	QwtPlotMagnifier::widgetMouseMoveEvent(_event);
}

void ot::CartesianPlotMagnifier::widgetMouseReleaseEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		if (m_mouseMoved) {
			m_marker->setVisible(false);
			m_plot->replot();
		}
		m_rightMouseIsPressed = false;
	}
	QwtPlotMagnifier::widgetMouseReleaseEvent(_event);
}

void ot::CartesianPlotMagnifier::widgetWheelEvent(QWheelEvent* _wheelEvent) {
	m_cursorPos = _wheelEvent->position();

	if (_wheelEvent->modifiers() != wheelModifiers()) { return; }

	if (wheelFactor() != 0.0) {
		int delta = _wheelEvent->angleDelta().y() * (-1);
		double f = qPow(wheelFactor(),
			qAbs(delta / 120.0));

		if (delta > 0)
			f = 1 / f;

		this->rescale(f);
	}

}

void ot::CartesianPlotMagnifier::rescale(double _factor) {
	QwtPlot* plt = plot();
	if (plt == nullptr)
		return;

	_factor = qAbs(_factor);
	if (_factor == 1.0 || _factor == 0.0) { return; }

	bool doReplot = false;

	const bool autoReplot = plt->autoReplot();
	plt->setAutoReplot(false);

	for (int axisId = 0; axisId < QwtPlot::axisCnt; axisId++) {
		if (isAxisEnabled(axisId)) {
			const QwtScaleMap scaleMap = plt->canvasMap(axisId);

			double v1 = scaleMap.s1(); //v1 is the bottom value of the axis scale
			double v2 = scaleMap.s2(); //v2 is the top value of the axis scale

			if (scaleMap.transformation()) {
				// the coordinate system of the paint device is always linear
				v1 = scaleMap.transform(v1); // scaleMap.p1()
				v2 = scaleMap.transform(v2); // scaleMap.p2()
			}

			double c = 0; //represent the position of the cursor in the axis coordinates
			if (axisId == QwtPlot::xBottom) //we only work with these two axis
			{
				c = scaleMap.invTransform(m_cursorPos.x());
				if (scaleMap.transformation()) {
					c = scaleMap.transform(c); // Revert the transformation in case that the currently used scale engine is using a log scale
				}

			}
			if (axisId == QwtPlot::yLeft) {
				c = scaleMap.invTransform(m_cursorPos.y());
				if (scaleMap.transformation()) {
					c = scaleMap.transform(c); // Revert the transformation in case that the currently used scale engine is using a log scale
				}
			}
			const double center = 0.5 * (v1 + v2);
			const double width_2 = 0.5 * (v2 - v1) * _factor;
			const double newCenter = c - _factor * (c - center);

			v1 = newCenter - width_2;
			v2 = newCenter + width_2;

			if (scaleMap.transformation()) {
				v1 = scaleMap.invTransform(v1);
				v2 = scaleMap.invTransform(v2);
			}

			plt->setAxisScale(axisId, v1, v2);
			doReplot = true;
		}
	}

	plt->setAutoReplot(autoReplot);

	if (doReplot) {
		plt->replot();
	}
}