//! @file PolarPlotData.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/PolarPlotData.h"

ot::PolarPlotData::PolarPlotData(double* _azimuth, double* _radius, size_t _dataSize)
	: m_azimuth(_azimuth), m_radius(_radius), m_size(_dataSize)
{

}

ot::PolarPlotData::~PolarPlotData() {

}

QwtPointPolar ot::PolarPlotData::sample(size_t _i) const {
	if (_i < m_size) {
		return QwtPointPolar(m_azimuth[_i], m_radius[_i]);
	}
	else {
		OT_LOG_EAS("Index (" + std::to_string(_i) + ") out of range");
		return QwtPointPolar(0, 0);
	}
}

void ot::PolarPlotData::setData(double* _azimuth, double* _radius, size_t _dataSize) {
	m_azimuth = _azimuth;
	m_radius = _radius;
	m_size = _dataSize;
}
