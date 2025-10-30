// @otlicense
// File: PolarPlotData.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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