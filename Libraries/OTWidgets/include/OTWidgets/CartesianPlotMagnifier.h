// @otlicense
// File: CartesianPlotMagnifier.h
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

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_plot_magnifier.h>

// Qt header
#include <QtCore/qpoint.h>

namespace ot {

	class CartesianPlot;
	class CartesianPlotTextMarker;
	class CartesianPlotCrossMarker;

	class OT_WIDGETS_API_EXPORT CartesianPlotMagnifier : public QwtPlotMagnifier {
	public:
		CartesianPlotMagnifier(CartesianPlot* _plot);

		virtual void widgetMousePressEvent(QMouseEvent* _event) override;

		virtual void widgetMouseMoveEvent(QMouseEvent* _event) override;

		virtual void widgetMouseReleaseEvent(QMouseEvent* _event) override;

		virtual void widgetWheelEvent(QWheelEvent* _wheelEvent) override;

		virtual void rescale(double _factor) override;

		virtual void widgetKeyPressEvent(QKeyEvent* _event) override;

	private:
		QPointF m_cursorPos;
		bool m_rightMouseIsPressed;
		CartesianPlot* m_plot;
		CartesianPlotCrossMarker* m_crossMarker;
		CartesianPlotTextMarker* m_textMarker;

		void updateMarkers(const QPoint& _pos);

	};

}