// @otlicense
// File: CartesianPlotDatasetData.cpp
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

// OpenTwin header
#include "OTWidgets/PlotDatasetData.h"
#include "OTWidgets/CartesianPlotDatasetData.h"

ot::CartesianPlotDatasetData::CartesianPlotDatasetData(PlotDatasetData* _data) 
	: m_data(_data)
{
	OTAssertNullptr(m_data);
}

ot::CartesianPlotDatasetData::~CartesianPlotDatasetData() {
	if (m_data) {
		m_data->forgetCartesianAccessor();
	}
}

size_t ot::CartesianPlotDatasetData::size() const {
	if (m_data) {
		return m_data->getSize();
	}
	else {
		return 0;
	}
}

QPointF ot::CartesianPlotDatasetData::sample(size_t _index) const {
	if (m_data) {
		return m_data->getSample<QPointF>(_index);
	}
	else {
		return QPointF();
	}
}
