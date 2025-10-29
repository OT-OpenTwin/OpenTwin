// @otlicense

//! @file PolarPlotMarker.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_polar_marker.h>

namespace ot {

	class PolarPlot;

	class OT_WIDGETS_API_EXPORT PolarPlotMarker : public QwtPolarMarker {
		OT_DECL_NOCOPY(PolarPlotMarker)
		OT_DECL_NODEFAULT(PolarPlotMarker)
	public:
		PolarPlotMarker(PolarPlot* _plot);
		virtual ~PolarPlotMarker();

	private:
		PolarPlot* m_plot;
	};

}