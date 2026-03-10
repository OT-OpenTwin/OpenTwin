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

#pragma once

// OpenTwin header
#include "OTWidgets/Plot/PlotSeriesData.h"
#include "OTWidgets/Plot/PlotDatasetData.h"

template<typename T>
inline ot::PlotSeriesData<T>::PlotSeriesData(PlotDatasetData* _data)
	: m_data(_data) {
	OTAssertNullptr(m_data);
}

template<typename T>
inline ot::PlotSeriesData<T>::~PlotSeriesData() {}

template<>
inline ot::PlotSeriesData<QPointF>::~PlotSeriesData() {
	if (m_data) {
		m_data->forgetCartesianAccessor();
	}
}

template<>
inline ot::PlotSeriesData<QwtPointPolar>::~PlotSeriesData() {
	if (m_data) {
		m_data->forgetPolarAccessor();
	}
}

template<typename T>
inline size_t ot::PlotSeriesData<T>::size() const {
	if (m_data) {
		return m_data->getSize();
	}
	else {
		return 0;
	}
}

template<typename T>
inline T ot::PlotSeriesData<T>::sample(size_t _index) const {
	if (m_data) {
		return m_data->getSample<T>(_index);
	}
	else {
		return T();
	}
}
