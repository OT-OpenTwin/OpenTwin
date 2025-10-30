// @otlicense
// File: CartesianPlotZoomer.h
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
#include <qwt_plot_zoomer.h>

namespace ot {

	class CartesianPlot;

	class OT_WIDGETS_API_EXPORT CartesianPlotZoomer : public QwtPlotZoomer {
	public:
		CartesianPlotZoomer(CartesianPlot* _plot);

		virtual void widgetMousePressEvent(QMouseEvent* _event) override;

		virtual void widgetMouseDoubleClickEvent(QMouseEvent* _event) override;

		virtual void widgetMouseMoveEvent(QMouseEvent* _event) override;

		virtual void widgetWheelEvent(QWheelEvent* _event) override;

		virtual void widgetMouseReleaseEvent(QMouseEvent* _event) override;

		virtual void widgetKeyPressEvent(QKeyEvent* _event) override;

		virtual void widgetKeyReleaseEvent(QKeyEvent* _event) override;

		bool isControlPressed(void) const { return m_controlIsPressed; }

	private:
		CartesianPlot* m_plot;
		bool           m_mouseMoved;
		bool           m_controlIsPressed;
	};

}