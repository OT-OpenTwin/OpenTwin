// @otlicense
// File: PolarPlotData.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/PolarPlotData.h"

ot::PolarPlotData::PolarPlotData(const double* _phase, const double* _magnitude, size_t _numberOfEntries) :
	m_azimuth(_phase), m_radius(_magnitude), m_numberOfEntries(_numberOfEntries)
{
	assert(m_azimuth != nullptr && m_radius != nullptr);
	assert(m_numberOfEntries >= 0);
}

QwtPointPolar ot::PolarPlotData::sample(size_t _i) const {
	if (_i < m_numberOfEntries) {
		return QwtPointPolar(m_azimuth[_i], m_radius[_i]);
	}
	else {
		OT_LOG_EAS("Index (" + std::to_string(_i) + ") out of range");
		return QwtPointPolar(0, 0);
	}
}

size_t ot::PolarPlotData::size() const
{
	return m_numberOfEntries;
}
