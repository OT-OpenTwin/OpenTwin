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
#include <qwt_polar_curve.h>

namespace ot {

	class PolarPlotGrid;
	class PolarPlotLegend;
	class PolarPlotPanner;
	class PolarPlotMagnifier;

	class OT_WIDGETS_API_EXPORT PolarPlot : public QwtPolarPlot, public AbstractPlot {
		Q_OBJECT
		OT_DECL_NOCOPY(PolarPlot)
		OT_DECL_NOMOVE(PolarPlot)
		OT_DECL_NODEFAULT(PolarPlot)
	public:
		PolarPlot(PlotBase* _owner, QWidget* _parent);
		virtual ~PolarPlot();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Plot

		virtual QWidget* getQWidget() override { return this; }
		virtual const QWidget* getQWidget() const override { return this; };

		virtual void updateLegend() override;

		virtual void updateWholePlot() override;

		virtual void clearPlot() override;

		virtual void resetPlotView() override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Grid

		virtual void updateGrid() override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		virtual Plot1DCfg::PlotType getPlotType() const override { return Plot1DCfg::Polar; };

		//! @brief Find the curve nearest to the provided position.
		//! @param _pos Position to find the nearest curve for.
		//! @param _pointIx Index of the point in the curve that is nearest to the provided position will be set here. Will not be initialized
		//! @return Pointer to the nearest curve, or nullptr if no curve is near enough.
		OT_DECL_NODISCARD QwtPolarCurve* findNearestCurve(const QwtPointPolar& _pos, size_t& _pointIx);

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;

		virtual void mouseMoveEvent(QMouseEvent* _event) override;

		virtual void leaveEvent(QEvent* _event) override;

	private Q_SLOTS:
		void slotColorStyleChanged();

	private:
		PolarPlotGrid* m_grid;
		PolarPlotLegend* m_legend;
		PolarPlotMagnifier* m_magnifier;
		PolarPlotPanner* m_panner;
	};

}