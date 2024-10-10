//! @file CartesianPlotMarker.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_plot_marker.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT CartesianPlotMarker : public QwtPlotMarker {
	public:
		CartesianPlotMarker(int _id);
		virtual ~CartesianPlotMarker();

		int getId(void) const { return m_id; };

		void setStyle(const QColor& _innerColor, const QColor& _outerColor, int _size = 8, double _outerColorSize = 2.0);

	private:
		int			m_id;
		QwtSymbol* m_symbol;

		CartesianPlotMarker() = delete;
		CartesianPlotMarker(CartesianPlotMarker&) = delete;
		CartesianPlotMarker& operator = (CartesianPlotMarker&) = delete;

	};

}