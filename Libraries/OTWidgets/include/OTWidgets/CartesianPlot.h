// @otlicense

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
		CartesianPlot(PlotBase* _owner);
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