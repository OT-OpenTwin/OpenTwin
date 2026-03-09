// @otlicense
// File: CartesianPlotMagnifier.cpp
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
#include "OTCore/Logging/LogDispatcher.h"
#include "OTWidgets/Style/GlobalColorStyle.h"
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/PlotDataset.h"
#include "OTWidgets/Plot/CartesianPlot.h"
#include "OTWidgets/Plot/CartesianPlotMagnifier.h"
#include "OTWidgets/Plot/CartesianPlotTextMarker.h"
#include "OTWidgets/Plot/CartesianPlotCrossMarker.h"

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

	m_textMarker = new CartesianPlotTextMarker;
	m_textMarker->attach(m_plot);
	m_textMarker->setVisible(false);

	this->setMouseButton(Qt::MouseButton::NoButton);

	slotColorStyleChanged();
	connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &CartesianPlotMagnifier::slotColorStyleChanged);
}

ot::CartesianPlotMagnifier::~CartesianPlotMagnifier() {
	disconnect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &CartesianPlotMagnifier::slotColorStyleChanged);
}

void ot::CartesianPlotMagnifier::rescale(double _factor) {
	QwtPlot* plt = this->plot();

	if (plt == nullptr) {
		return;
	}

	_factor = qAbs(_factor);

	if (_factor == 1.0 || _factor == 0.0) {
		return;
	}

	bool doReplot = false;

	const bool autoReplot = plt->autoReplot();
	plt->setAutoReplot(false);

	for (int axisId = 0; axisId < QwtPlot::axisCnt; axisId++) {
		if (!isAxisEnabled(axisId)) {
			continue;
		}

		const QwtScaleMap scaleMap = plt->canvasMap(axisId);

		double axisBottom = scaleMap.s1();
		double axisTop = scaleMap.s2();

		// In case that the currently used scale engine is using a log scale,
		// we need to transform the axis limits to linear coordinates before calculating the new axis limits.
		// After calculating the new axis limits, 
		// we need to revert the transformation to set the new axis limits in the correct coordinate system.

		if (scaleMap.transformation()) {
			axisBottom = scaleMap.transform(axisBottom);
			axisTop = scaleMap.transform(axisTop);
		}

		double cursorPosition = 0;

		// We only work with xBottom and yLeft axis
		if (axisId == QwtPlot::xBottom)
		{
			cursorPosition = scaleMap.invTransform(m_cursorPos.x());

			// Revert the transformation in case that the currently used scale engine is using a log scale
			if (scaleMap.transformation()) {
				cursorPosition = scaleMap.transform(cursorPosition);
			}

		}
		if (axisId == QwtPlot::yLeft) {
			cursorPosition = scaleMap.invTransform(m_cursorPos.y());

			// Revert the transformation in case that the currently used scale engine is using a log scale
			if (scaleMap.transformation()) {
				cursorPosition = scaleMap.transform(cursorPosition);
			}
		}
		const double center = 0.5 * (axisBottom + axisTop);
		const double width_2 = 0.5 * (axisTop - axisBottom) * _factor;
		const double newCenter = cursorPosition - _factor * (cursorPosition - center);

		axisBottom = newCenter - width_2;
		axisTop = newCenter + width_2;

		if (scaleMap.transformation()) {
			axisBottom = scaleMap.invTransform(axisBottom);
			axisTop = scaleMap.invTransform(axisTop);
		}

		plt->setAxisScale(axisId, axisBottom, axisTop);
		doReplot = true;
	}

	plt->setAutoReplot(autoReplot);

	if (doReplot) {
		plt->replot();
	}
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

	if (_wheelEvent->modifiers() != wheelModifiers()) { 
		return;
	}

	if (wheelFactor() != 0.0) {
		int delta = _wheelEvent->angleDelta().y() * (-1);
		double factor = qPow(wheelFactor(), qAbs(delta / 120.0));

		if (delta > 0) {
			factor = 1 / factor;
		}

		this->rescale(factor);
	}
}

void ot::CartesianPlotMagnifier::widgetKeyPressEvent(QKeyEvent* _event) {
	if (_event->key() == Qt::Key_Space) {
		m_plot->getOwner()->resetView();
	}
}

void ot::CartesianPlotMagnifier::slotColorStyleChanged() {
	const auto& style = GlobalColorStyle::instance().getCurrentStyle();
	const auto& lineVal = style.getValue(ColorStyleValueEntry::PlotMarkerLine, ColorStyleValue());
	const auto& textVal = style.getValue(ColorStyleValueEntry::PlotMarkerText, ColorStyleValue());
	
	m_crossMarker->setLinePen(QPen(lineVal.toBrush(), 0.0, Qt::DashDotDotLine));
	m_textMarker->setLinePen(QPen(textVal.toBrush(), 0.0, Qt::SolidLine));
}

void ot::CartesianPlotMagnifier::updateMarkers(const QPoint& _pos) {
	int itemIx;
	QwtPlotCurve* curve = m_plot->findNearestCurve(_pos, itemIx);
	
	if (curve != nullptr) {
		// Find dataset
		OTAssertNullptr(m_plot->getOwner());

		PlotDataset* dataset = m_plot->getOwner()->findDataset(curve);
		OTAssertNullptr(dataset);
		const PlotDatasetData& datasetData = dataset->getPlotData();
		QPointF pt;
		if (itemIx < 0 || itemIx >= datasetData.getSize())
		{
			return;
		}
		else
		{
			pt = datasetData.getSample<QPointF>(itemIx);
		}
		
		// Set new label
		QwtText newText(m_plot->getOwner()->toPositionInfoText(pt, true), QwtText::PlainText);
		newText.setColor(QColor(255, 50, 50));

		// Get canvas rect and transform maps
		const QwtScaleMap xMap = m_plot->canvasMap(QwtPlot::xBottom);
		const QwtScaleMap yMap = m_plot->canvasMap(QwtPlot::yLeft);
		const QRectF canvasRect(QPointF(xMap.s1(), yMap.s1()), QPointF(xMap.s2(), yMap.s2()));

		// Determine text size in canvas
		QSizeF textSize = newText.textSize();
		QPointF textTopLeft(pt);
		
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
		QPointF adjustedPt = pt;

		if (textBoundingRect.right() > canvasRect.right()) {
			adjustedPt.setX(adjustedPt.x() - (textBoundingRect.right() - canvasRect.right()));
		}
		if (textBoundingRect.left() < canvasRect.left()) {
			adjustedPt.setX(adjustedPt.x() + (canvasRect.left() - textBoundingRect.left()));
		}
		if (textBoundingRect.bottom() > canvasRect.bottom()) {
			adjustedPt.setY(adjustedPt.y() - (textBoundingRect.bottom() - canvasRect.bottom()));
		}
		if (textBoundingRect.top() < canvasRect.top()) {
			adjustedPt.setY(adjustedPt.y() + (canvasRect.top() - textBoundingRect.top()));
		}

		// Apply
		m_crossMarker->setValue(pt);
		m_textMarker->setValue(adjustedPt);
		m_textMarker->setLabel(newText);
		m_plot->replot();
	}
}
