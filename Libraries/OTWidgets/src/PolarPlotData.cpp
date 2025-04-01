//! @file PolarPlotData.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
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
