// @otlicense
// File: CartesianPlot.h
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
#include "OTWidgets/AbstractPlot.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

class QwtPlotCurve;

namespace ot {

	class CartesianPlotGrid;
	class CartesianPlotPanner;
	class CartesianPlotLegend;
	class CartesianPlotPicker;
	class CartesianPlotZoomer;
	class CartesianPlotMagnifier;
	class CartesianPlotWidgetCanvas;

	class OT_WIDGETS_API_EXPORT CartesianPlot : public QwtPlot, public AbstractPlot {
		OT_DECL_NOCOPY(CartesianPlot)
		OT_DECL_NOMOVE(CartesianPlot)
		OT_DECL_NODEFAULT(CartesianPlot)
	public:
		CartesianPlot(PlotBase* _owner, QWidget* _parent);
		virtual ~CartesianPlot();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void updateLegend(void) override;

		virtual void updateWholePlot(void) override;

		virtual void clearPlot(void) override;

		void setZoomerPen(const QPen& _pen);

		virtual void resetPlotView(void) override;

		virtual void mouseDoubleClickEvent(QMouseEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Grid

		virtual void updateGrid(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		virtual Plot1DCfg::PlotType getPlotType(void) const override { return Plot1DCfg::Cartesian; };

		QwtPlotCurve* findNearestCurve(const QPoint& _pos, int& _pointIx);

	private:
		CartesianPlotZoomer* m_plotZoomer  = nullptr;
		CartesianPlotMagnifier* m_plotMagnifier = nullptr;
		CartesianPlotPanner* m_plotPanner = nullptr;
		CartesianPlotWidgetCanvas* m_canvas = nullptr;
		CartesianPlotLegend* m_legend = nullptr;
		CartesianPlotGrid* m_grid = nullptr;
		CartesianPlotPicker* m_picker = nullptr;
	};

}