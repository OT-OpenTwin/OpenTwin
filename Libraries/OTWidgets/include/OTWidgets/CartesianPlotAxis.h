// @otlicense

//! @file CartesianPlotAxis.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/AbstractPlotAxis.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	class CartesianPlot;

	class OT_WIDGETS_API_EXPORT CartesianPlotAxis : public AbstractPlotAxis {
		OT_DECL_NOCOPY(CartesianPlotAxis)
		OT_DECL_NODEFAULT(CartesianPlotAxis)
	public:
		CartesianPlotAxis(AxisID _axisID, CartesianPlot* _plot);
		virtual ~CartesianPlotAxis();

		virtual void updateAxis(void) override;

	private:
		CartesianPlot* m_plot;
	};

}