// @otlicense
// File: CartesianPlotZoomer.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/PlotBase.h"
#include "OTWidgets/CartesianPlot.h"
#include "OTWidgets/CartesianPlotZoomer.h"

// Qt header
#include <QtGui/qevent.h>

ot::CartesianPlotZoomer::CartesianPlotZoomer(CartesianPlot* _plot)
	: QwtPlotZoomer(_plot->canvas()), m_plot(_plot), m_mouseMoved(false), m_controlIsPressed(false)
{
	OTAssertNullptr(m_plot);

}

void ot::CartesianPlotZoomer::widgetMousePressEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::MouseButton::LeftButton) {
		QwtPlotZoomer::widgetMousePressEvent(_event);
	}
	else if (_event->button() == Qt::MouseButton::RightButton) {
		m_mouseMoved = false;
		QwtPlotZoomer::widgetMousePressEvent(_event);
	}
}

void ot::CartesianPlotZoomer::widgetMouseDoubleClickEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::MouseButton::LeftButton) {
		QwtPlotZoomer::widgetMouseDoubleClickEvent(_event);
	}
}

void ot::CartesianPlotZoomer::widgetMouseMoveEvent(QMouseEvent* _event) {
	m_mouseMoved = true;
	QwtPlotZoomer::widgetMouseMoveEvent(_event);
}

void ot::CartesianPlotZoomer::widgetWheelEvent(QWheelEvent* _event) {

}

void ot::CartesianPlotZoomer::widgetMouseReleaseEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::MouseButton::LeftButton) {
		QwtPlotZoomer::widgetMouseReleaseEvent(_event);
	}
	else if (_event->button() == Qt::MouseButton::RightButton) {
		if (!m_mouseMoved) {
			QwtPlotZoomer::widgetMouseReleaseEvent(_event);
		}
	}
}

void ot::CartesianPlotZoomer::widgetKeyPressEvent(QKeyEvent* _event) {
	if (_event->key() == Qt::Key::Key_Control) {
		m_controlIsPressed = true;
	}
}

void ot::CartesianPlotZoomer::widgetKeyReleaseEvent(QKeyEvent* _event) {
	if (_event->key() == Qt::Key::Key_Control) {
		m_controlIsPressed = false;
	}
}

