//! @file AbstractPlotAxis.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

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
	public:
		CartesianPlot(Plot* _owner);
		virtual ~CartesianPlot();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Plot

		virtual void updateLegend(void) override;

		virtual void updateWholePlot(void) override;

		virtual void clearPlot(void) override;

		void setZoomerPen(const QPen& _pen);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Grid

		virtual void updateGrid(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		virtual Plot1DCfg::PlotType getPlotType(void) const override { return Plot1DCfg::Cartesian; };

		QwtPlotCurve* findNearestCurve(const QPoint& _pos, int& _pointID);

	private:
		CartesianPlotZoomer* m_plotZoomer;
		CartesianPlotMagnifier* m_plotMagnifier;
		CartesianPlotPanner* m_plotPanner;
		CartesianPlotWidgetCanvas* m_canvas;
		CartesianPlotLegend* m_legend;
		CartesianPlotGrid* m_grid;
		CartesianPlotPicker* m_picker;
	};

}