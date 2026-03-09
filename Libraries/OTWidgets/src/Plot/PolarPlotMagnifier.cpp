// @otlicense
// File: PolarPlotMagnifier.cpp
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
#include "OTCore/Symbol.h"
#include "OTWidgets/Plot/PolarPlot.h"
#include "OTWidgets/Plot/PolarPlotMarker.h"
#include "OTWidgets/Plot/PolarPlotMagnifier.h"
#include "OTWidgets/Style/GlobalColorStyle.h"

// Qwt header
#include <qwt_polar_canvas.h>

// Qt header
#include <QtGui/qevent.h>

ot::PolarPlotMagnifier::PolarPlotMagnifier(PolarPlot* _plot)
	: QwtPolarMagnifier(_plot->canvas()), m_state(State::None), m_plot(_plot) 
{
	this->setMouseButton(Qt::MouseButton::NoButton);
	this->setEnabled(true);
	m_marker = new PolarPlotMarker(m_plot);
	m_marker->setVisible(false);

	slotColorStyleChanged();
	connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &PolarPlotMagnifier::slotColorStyleChanged);
}

ot::PolarPlotMagnifier::~PolarPlotMagnifier() {
	disconnect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &PolarPlotMagnifier::slotColorStyleChanged);
}

void ot::PolarPlotMagnifier::widgetMousePressEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		m_state.set(State::RightMousePressed);
		updateMarker(_event->pos());
	}
	QwtPolarMagnifier::widgetMousePressEvent(_event);
}

void ot::PolarPlotMagnifier::widgetMouseMoveEvent(QMouseEvent* _event) {
	if (m_state.has(State::MarkerShown)) {
		updateMarker(_event->pos());
	}
	QwtPolarMagnifier::widgetMouseMoveEvent(_event);
}

void ot::PolarPlotMagnifier::widgetMouseReleaseEvent(QMouseEvent* _event) {
	if (m_state.has(State::MarkerShown)) {
		if (_event->button() == Qt::MouseButton::RightButton) {
			m_state.remove(State::RightMousePressed);
			hideMarker();
		}
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

void ot::PolarPlotMagnifier::slotColorStyleChanged() {
	const auto& style = GlobalColorStyle::instance().getCurrentStyle();
	const auto& textVal = style.getValue(ColorStyleValueEntry::PlotMarkerText, ColorStyleValue());

	m_markerText.setColor(textVal.toColor());
}

void ot::PolarPlotMagnifier::updateMarker(const QPoint& _pos) {
	const QwtPolarCanvas* canvas = m_plot->canvas();
	QwtPointPolar polar = canvas->invTransform(_pos);

	size_t ix = 0;
	QwtPolarCurve* curve = m_plot->findNearestCurve(polar, ix);

	if (!curve) {
		return;
	}

	polar = curve->data()->sample(ix);

	double radius = polar.radius();
	double azimuthRad = polar.azimuth();
	double azimuthDeg = qRadiansToDegrees(azimuthRad);

	m_markerText.setText("r = " + QString::number(radius) + "\n" +
		QString::fromUtf8(Symbol::phi()) + " = " + QString::number(azimuthRad) + " rad\n" +
		QString::fromUtf8(Symbol::phi()) + " =" + QString::number(azimuthDeg) + " deg"
	);

	m_state.set(State::MarkerShown);

	m_marker->setPosition(polar);
	m_marker->setLabel(m_markerText);
	m_marker->setVisible(true);

	m_plot->replot();
}

void ot::PolarPlotMagnifier::hideMarker() {
	m_state.remove(State::MarkerShown);
	m_marker->setVisible(false);
	m_plot->replot();
}
