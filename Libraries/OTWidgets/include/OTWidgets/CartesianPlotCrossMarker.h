//! @file CartesianPlotCrossMarker.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_plot_marker.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT CartesianPlotCrossMarker : public QwtPlotMarker {
		OT_DECL_NOCOPY(CartesianPlotCrossMarker)
	public:
		CartesianPlotCrossMarker();
		virtual ~CartesianPlotCrossMarker();

		void setStyle(const QColor& _innerColor, const QColor& _outerColor, int _size = 8, double _outerColorSize = 2.0);

	private:
		QwtSymbol* m_symbol;
	};

}