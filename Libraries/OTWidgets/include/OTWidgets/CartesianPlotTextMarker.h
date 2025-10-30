// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_plot_marker.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT CartesianPlotTextMarker : public QwtPlotMarker {
		OT_DECL_NOCOPY(CartesianPlotTextMarker)
	public:
		CartesianPlotTextMarker();
		virtual ~CartesianPlotTextMarker();
	};

}