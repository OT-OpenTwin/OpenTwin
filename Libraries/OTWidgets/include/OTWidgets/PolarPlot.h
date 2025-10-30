// @otlicense
// File: PolarPlot.h
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

// Qwt header
#include <qwt_polar_plot.h>

namespace ot {

	class PolarPlotGrid;
	class PolarPlotLegend;
	class PolarPlotPanner;
	class PolarPlotMagnifier;

	class OT_WIDGETS_API_EXPORT PolarPlot : public QwtPolarPlot, public AbstractPlot {
		OT_DECL_NOCOPY(PolarPlot)
		OT_DECL_NODEFAULT(PolarPlot)
	public:
		PolarPlot(PlotBase* _owner);
		virtual ~PolarPlot();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Plot

		virtual void updateLegend(void) override;

		virtual void updateWholePlot(void) override;

		virtual void clearPlot(void) override;

		virtual void resetPlotView(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Grid

		virtual void updateGrid(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		virtual Plot1DCfg::PlotType getPlotType(void) const override { return Plot1DCfg::Polar; };

	private:
		PolarPlotGrid* m_grid;
		PolarPlotLegend* m_legend;
		PolarPlotMagnifier* m_magnifier;
		PolarPlotPanner* m_panner;
	};

}