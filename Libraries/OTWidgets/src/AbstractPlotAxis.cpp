// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/AbstractPlotAxis.h"

ot::AbstractPlotAxis::AbstractPlotAxis(AxisID _id) :
	m_id{ _id }, m_isLogScaleSet(false)
{}

ot::AbstractPlotAxis::~AbstractPlotAxis() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::AbstractPlotAxis::setIsAutoScale(bool _isAutoScale) {
	m_config.setIsAutoScale(_isAutoScale);
}

bool ot::AbstractPlotAxis::getIsAutoScale(void) const {
	return m_config.getIsAutoScale();
}

void ot::AbstractPlotAxis::setIsLogScale(bool _isLogScale) {
	m_config.setIsLogScale(_isLogScale);
}

bool ot::AbstractPlotAxis::getIsLogScale(void) const {
	return m_config.getIsLogScale();
}

void ot::AbstractPlotAxis::setMin(double _minValue) {
	m_config.setMin(_minValue);
}

double ot::AbstractPlotAxis::getMin(void) const {
	return m_config.getMin();
}

void ot::AbstractPlotAxis::setMax(double _maxValue) {
	m_config.setMax(_maxValue);
}

double ot::AbstractPlotAxis::getMax(void) const {
	return m_config.getMax();
}

QwtPlot::Axis ot::AbstractPlotAxis::getCartesianAxisID(void) const {
	switch (m_id) {
	case AbstractPlotAxis::yLeft: return QwtPlot::yLeft;
	case AbstractPlotAxis::yRight: return QwtPlot::yRight;
	case AbstractPlotAxis::xBottom: return QwtPlot::xBottom;
	case AbstractPlotAxis::xTop: return QwtPlot::xTop;
	default:
		assert(0);	// Not implemented axis
		return QwtPlot::xBottom;
	}
}

QwtPolar::Axis ot::AbstractPlotAxis::getPolarAxisID(void) const {
	switch (m_id) {
	case AbstractPlotAxis::yLeft: return QwtPolar::AxisLeft;
	case AbstractPlotAxis::yRight: return QwtPolar::AxisRight;
	case AbstractPlotAxis::xBottom: return QwtPolar::AxisBottom;
	case AbstractPlotAxis::xTop: return QwtPolar::AxisTop;
	default:
		assert(0);	// Not implemented axis
		return QwtPolar::AxisBottom;
	}
}
