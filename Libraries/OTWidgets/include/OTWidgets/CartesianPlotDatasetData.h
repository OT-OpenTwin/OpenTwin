// @otlicense
// File: CartesianPlotDatasetData.h
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
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_series_data.h>

namespace ot {

	class PlotDatasetData;

	class CartesianPlotDatasetData : public QwtSeriesData<QPointF> {
		OT_DECL_NOCOPY(CartesianPlotDatasetData)
		OT_DECL_NOMOVE(CartesianPlotDatasetData)
		OT_DECL_NODEFAULT(CartesianPlotDatasetData)
	public:
		CartesianPlotDatasetData(PlotDatasetData* _data);
		virtual ~CartesianPlotDatasetData();

		virtual size_t size() const override;
		virtual QPointF sample(size_t _index) const override;

	private:
		friend class PlotDatasetData;
		void forgetData() { m_data = nullptr; };

		PlotDatasetData* m_data;
	};
}
