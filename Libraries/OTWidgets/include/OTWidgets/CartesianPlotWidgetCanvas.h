// @otlicense

//! @file CartesianPlotWidgetCanvas.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_plot_canvas.h>

namespace ot {

	class CartesianPlot;

	class OT_WIDGETS_API_EXPORT CartesianPlotWidgetCanvas : public QwtPlotCanvas {
		//Q_OBJECT
	public:
		CartesianPlotWidgetCanvas(CartesianPlot* _plot);
		virtual ~CartesianPlotWidgetCanvas();

	private:
		CartesianPlot* m_plot;
	};

}