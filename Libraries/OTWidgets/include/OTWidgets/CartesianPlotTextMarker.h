//! @file CartesianPlotTextMarker.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

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