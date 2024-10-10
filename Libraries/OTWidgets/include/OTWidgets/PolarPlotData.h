//! @file PolarPlotData.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_series_data.h>
#include <qwt_point_polar.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PolarPlotData : public QwtSeriesData<QwtPointPolar> {
	public:
		PolarPlotData(double* _azimuth, double* _radius, size_t _dataSize);
		virtual ~PolarPlotData();

		virtual size_t size(void) const override { return m_size; }

		virtual QwtPointPolar sample(size_t _i) const override;

		void replaceData(double* _azimuth, double* _radius, size_t _dataSize);

	private:
		void clear(void);

		double* m_azimuth;
		double* m_radius;
		size_t m_size;
	};

}