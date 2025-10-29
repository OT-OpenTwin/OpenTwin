// @otlicense

//! @file CartesianPlotGrid.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_plot_grid.h>

// Qt header
#include <QtGui/qpen.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT CartesianPlotGrid : public QwtPlotGrid {
	public:
		CartesianPlotGrid();
		virtual ~CartesianPlotGrid();

		void setGrid(const QColor& _color, double _width);
		void setGrid(const QPen& _pen);
	};

}