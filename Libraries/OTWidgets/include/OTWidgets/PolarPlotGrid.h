//! @file PolarPlotGrid.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_polar_grid.h>

namespace ot {

	class PolarPlot;

	class OT_WIDGETS_API_EXPORT PolarPlotGrid : public QwtPolarGrid {
		OT_DECL_NOCOPY(PolarPlotGrid)
		OT_DECL_NODEFAULT(PolarPlotGrid)
	public:
		PolarPlotGrid(PolarPlot* _plot);
		virtual ~PolarPlotGrid();

	private:
		PolarPlot* m_plot;
	};

}