// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_legend.h>

namespace ot {

	class PolarPlot;

	class OT_WIDGETS_API_EXPORT PolarPlotLegend : public QwtLegend {
		OT_DECL_NOCOPY(PolarPlotLegend)
		OT_DECL_NODEFAULT(PolarPlotLegend)
	public:
		PolarPlotLegend(PolarPlot* _plot);
		virtual ~PolarPlotLegend();

	private:
		PolarPlot* m_plot;
	};

}