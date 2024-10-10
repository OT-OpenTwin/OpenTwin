//! @file CartesianPlotLegend.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_legend.h>

namespace ot {

	class CartesianPlot;

	class OT_WIDGETS_API_EXPORT CartesianPlotLegend : public QwtLegend {
	public:
		explicit CartesianPlotLegend(CartesianPlot* _owner);
		virtual ~CartesianPlotLegend();

	private:
		CartesianPlot* m_owner;

	};

}