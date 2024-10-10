//! @file CartesianPlotPicker.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_plot_picker.h>

namespace ot {

	class CartesianPlot;

	class OT_WIDGETS_API_EXPORT CartesianPlotPicker : public QwtPlotPicker {
	public:
		explicit CartesianPlotPicker(CartesianPlot* _plot);
		virtual ~CartesianPlotPicker();

	private:
		CartesianPlot* m_plot;
	};

}