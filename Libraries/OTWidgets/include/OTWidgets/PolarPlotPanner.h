//! @file PolarPlotPanner.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_polar_panner.h>

namespace ot {

	class PolarPlot;

	class OT_WIDGETS_API_EXPORT PolarPlotPanner : public QwtPolarPanner {
		OT_DECL_NOCOPY(PolarPlotPanner)
		OT_DECL_NODEFAULT(PolarPlotPanner)
	public:
		PolarPlotPanner(PolarPlot* _plot);
		virtual ~PolarPlotPanner();

	private:
		PolarPlot* m_plot;
	};

}
