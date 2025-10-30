// @otlicense

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