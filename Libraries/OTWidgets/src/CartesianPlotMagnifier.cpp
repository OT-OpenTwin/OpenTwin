//! @file CartesianPlotMagnifier.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Plot.h"
#include "OTWidgets/PlotDataset.h"
#include "OTWidgets/CartesianPlot.h"
#include "OTWidgets/CartesianPlotMagnifier.h"
#include "OTWidgets/CartesianPlotTextMarker.h"
#include "OTWidgets/CartesianPlotCrossMarker.h"

// Qwt header
#include <qwt_text.h>
#include <qwt_scale_map.h>

// Qt header
#include <QtGui/qevent.h>

ot::CartesianPlotMagnifier::CartesianPlotMagnifier(CartesianPlot* _plot)
	: QwtPlotMagnifier(_plot->canvas()), m_plot(_plot), m_rightMouseIsPressed(false)
{
	m_crossMarker = new CartesianPlotCrossMarker;
	m_crossMarker->attach(m_plot);
	m_crossMarker->setVisible(false);
	m_crossMarker->setLinePen(QColor(255, 50, 50), 0.0, Qt::DashDotDotLine);

	m_textMarker = new CartesianPlotTextMarker;
	m_textMarker->attach(m_plot);
	m_textMarker->setVisible(false);
	m_textMarker->setLinePen(QColor(255, 50, 50), 0.0, Qt::DashDotDotLine);

	this->setMouseButton(Qt::MouseButton::NoButton);
}

void ot::CartesianPlotMagnifier::widgetMousePressEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		m_rightMouseIsPressed = true;
		m_crossMarker->setVisible(true);
		m_textMarker->setVisible(true);
		this->updateMarkers(_event->pos());
	}
	QwtPlotMagnifier::widgetMousePressEvent(_event);
}

void ot::CartesianPlotMagnifier::widgetMouseMoveEvent(QMouseEvent* _event) {
	if (m_rightMouseIsPressed) {
		this->updateMarkers(_event->pos());
	}
	QwtPlotMagnifier::widgetMouseMoveEvent(_event);
}

void ot::CartesianPlotMagnifier::widgetMouseReleaseEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		m_rightMouseIsPressed = false;
		m_crossMarker->setVisible(false);
		m_textMarker->setVisible(false);
		m_plot->replot();
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

void ot::CartesianPlotMagnifier::updateMarkers(const QPoint& _pos) {
	int itemId;
	QwtPlotCurve* curve = m_plot->findNearestCurve(_pos, itemId);

	if (curve != nullptr) {
		// Find dataset
		OTAssertNullptr(m_plot->getOwner());

		PlotDataset* dataset = m_plot->getOwner()->findDataset(curve);
		OTAssertNullptr(dataset);

		double x, y;
		if (!dataset->getDataAt(itemId, x, y)) {
			return;
		}

		// Set new label
		QwtText newText(QString::number(x) + "; " + QString::number(y), QwtText::PlainText);
		newText.setColor(QColor(255, 50, 50));

		// Get canvas rect and transform maps
		const QwtScaleMap xMap = m_plot->canvasMap(QwtPlot::xBottom);
		const QwtScaleMap yMap = m_plot->canvasMap(QwtPlot::yLeft);
		const QRectF canvasRect(QPointF(xMap.s1(), yMap.s1()), QPointF(xMap.s2(), yMap.s2()));

		// Determine text size in canvas
		QSizeF textSize = newText.textSize();
		QPointF textTopLeft(x, y);
		
		textSize.setWidth((xMap.invTransform(xMap.transform(0) + textSize.width())) - xMap.invTransform(xMap.transform(0)));
		textSize.setHeight((yMap.invTransform(yMap.transform(0) + textSize.height())) - yMap.invTransform(yMap.transform(0)));

		if (textSize.width() < 0.) {
			textSize.setWidth(textSize.width() * (-1.));
			textTopLeft.setX(textTopLeft.x() - textSize.width());
		}
		if (textSize.height() < 0.) {
			textSize.setHeight(textSize.height() * (-1.));
			textTopLeft.setY(textTopLeft.y() - textSize.height());
		}
		QRectF textBoundingRect(textTopLeft, textSize);

		// Ensure text rect is in canvas
		double adjustedX = x;
		double adjustedY = y;

		if (textBoundingRect.right() > canvasRect.right()) {
			adjustedX -= textBoundingRect.right() - canvasRect.right();
		}
		if (textBoundingRect.left() < canvasRect.left()) {
			adjustedX += canvasRect.left() - textBoundingRect.left();
		}
		if (textBoundingRect.bottom() > canvasRect.bottom()) {
			adjustedY -= textBoundingRect.bottom() - canvasRect.bottom();
		}
		if (textBoundingRect.top() < canvasRect.top()) {
			adjustedY += canvasRect.top() - textBoundingRect.top();
		}

		// Apply
		m_crossMarker->setValue(x, y);
		m_textMarker->setValue(adjustedX, adjustedY);
		m_textMarker->setLabel(newText);
		m_plot->replot();
	}
}
