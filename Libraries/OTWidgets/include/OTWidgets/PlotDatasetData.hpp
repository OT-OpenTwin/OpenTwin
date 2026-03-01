// @otlicense
// File: PlotDatasetData.hpp
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
#include "OTWidgets/PlotDatasetData.h"

inline size_t ot::PlotDatasetData::getSize() const {
	OTAssert(m_calcX.size() == m_calcY.size(), "Data size mismatch");
	return m_calcX.size();
}

template <typename T>
inline T ot::PlotDatasetData::getSample(size_t _index) const {
	OTAssert(m_calcX.size() == m_calcY.size(), "Data size mismatch");
	OTAssert(_index < m_calcX.size(), "Index out of range");
	return T(m_calcX.at(_index), m_calcY.at(_index));
}