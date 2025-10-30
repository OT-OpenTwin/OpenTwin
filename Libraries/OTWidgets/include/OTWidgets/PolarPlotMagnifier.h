// @otlicense
// File: PolarPlotMagnifier.h
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
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_polar_magnifier.h>

namespace ot {

	class PolarPlot;
	class PolarPlotMarker;

	class OT_WIDGETS_API_EXPORT PolarPlotMagnifier : public QwtPolarMagnifier {
		OT_DECL_NOCOPY(PolarPlotMagnifier)
		OT_DECL_NODEFAULT(PolarPlotMagnifier)
	public:
		PolarPlotMagnifier(PolarPlot* _plot);
		virtual ~PolarPlotMagnifier();

		virtual void widgetMousePressEvent(QMouseEvent* _event) override;

		virtual void widgetMouseMoveEvent(QMouseEvent* _event) override;

		virtual void widgetMouseReleaseEvent(QMouseEvent* _event) override;

		virtual void widgetWheelEvent(QWheelEvent* _wheelEvent) override;

		virtual void rescale(double _factor) override;


	private:
		PolarPlot* m_plot;
		bool				m_rightMouseIsPressed;
		bool				m_mouseMoved;
		PolarPlotMarker* m_marker;
	};

}