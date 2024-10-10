//! @file CartesianPlotPanner.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_plot_panner.h>

namespace ot {

	class CartesianPlot;

	class OT_WIDGETS_API_EXPORT CartesianPlotPanner : public QwtPlotPanner {
	public:
		CartesianPlotPanner(CartesianPlot* _plot);
		virtual ~CartesianPlotPanner();

		virtual bool eventFilter(QObject* _object, QEvent* _event) override;

	private:
		bool		m_mouseIsPressed;
	};

}