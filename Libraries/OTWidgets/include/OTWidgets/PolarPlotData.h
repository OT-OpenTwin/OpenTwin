// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_series_data.h>
#include <qwt_point_polar.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PolarPlotData : public QwtSeriesData<QwtPointPolar> {
	public:
		PolarPlotData(const double* _phase, const double* _magnitude,size_t _numberOfEntries);
	
		virtual QwtPointPolar sample(size_t _i) const override;

	private:
		//Expressions as they are more used in for the plot geometry. Magnitude ^= radius and in this context phase ^= azimuth
		const double* m_azimuth = nullptr;
		const double* m_radius = nullptr;
		size_t m_numberOfEntries = 0;

		size_t size() const override;
	};

}