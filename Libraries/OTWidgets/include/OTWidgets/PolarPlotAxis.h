// @otlicense

//! @file PolarPlotAxis.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/AbstractPlotAxis.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	class PolarPlot;

	class OT_WIDGETS_API_EXPORT PolarPlotAxis : public AbstractPlotAxis {
		OT_DECL_NOCOPY(PolarPlotAxis)
		OT_DECL_NODEFAULT(PolarPlotAxis)
	public:
		PolarPlotAxis(AxisID _axisID, PolarPlot* _plot);
		virtual ~PolarPlotAxis();

		virtual void updateAxis(void) override;

	private:
		PolarPlot* m_plot;
	};

}